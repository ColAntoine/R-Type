#include "Multiplayer.hpp"
#include "ServerECS/ServerECS.hpp"
#include <iostream>
#include "ECS/Components/InputBuffer.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/Components/Animation.hpp"
#include <chrono>
#include "../../Constants.hpp"

#include "Network/UDPServer.hpp"
#include "Network/Session.hpp"

namespace RType::Network {

Multiplayer::Multiplayer(ServerECS &ecs) : ecs_(ecs) {}
Multiplayer::~Multiplayer() = default;

void Multiplayer::set_udp_server(UdpServer* server) {
    udp_server_ = server;
    
    // Register game start callback to spawn all players when game starts
    if (udp_server_) {
        udp_server_->set_game_start_callback([this]() {
            this->spawn_all_players();
        });
    }
}

void Multiplayer::handle_packet(const std::string &session_id, const std::vector<char> &data) {
    if (data.empty()) return;
    uint8_t msg_type = static_cast<uint8_t>(data[0]);
    std::vector<char> payload;
    if (data.size() > 1) payload.insert(payload.end(), data.begin() + 1, data.end());

    std::cout << "Pkt type=" << int(msg_type) << " from " << session_id
              << " payload=" << payload.size() << std::endl;

    using RType::Protocol::SystemMessage;
    if (msg_type == static_cast<uint8_t>(SystemMessage::CLIENT_CONNECT)) {
        handle_client_connect(session_id, payload);
        return;
    }
    if (msg_type == static_cast<uint8_t>(SystemMessage::CLIENT_DISCONNECT)) {
        handle_client_disconnect(session_id, payload);
        return;
    }
    if (msg_type == static_cast<uint8_t>(SystemMessage::CLIENT_READY)) {
        handle_client_ready(session_id, payload);
        return;
    }
    if (msg_type == static_cast<uint8_t>(SystemMessage::CLIENT_UNREADY)) {
        handle_client_unready(session_id, payload);
        return;
    }

    // Check for PLAYER_INPUT game message
    using RType::Protocol::GameMessage;
    if (msg_type == static_cast<uint8_t>(GameMessage::PLAYER_INPUT)) {
        handle_player_input(session_id, payload);
        return;
    }

    // Otherwise treat as game message / input
    handle_game_message(session_id, msg_type, payload);
}

void Multiplayer::handle_client_connect(const std::string &session_id, const std::vector<char> &payload) {
    const RType::Protocol::ClientConnect* cc = nullptr;
    if (!payload.empty() && payload.size() >= sizeof(RType::Protocol::ClientConnect)) {
        cc = reinterpret_cast<const RType::Protocol::ClientConnect*>(payload.data());
    }

    // If this session already has a token, it's a duplicate/resent CONNECT - reply with existing token and return
    auto existing_tok_it = ecs_.session_token_map_.find(session_id);
    if (existing_tok_it != ecs_.session_token_map_.end()) {
        uint32_t existing_token = existing_tok_it->second;
        std::cout << "Duplicate CLIENT_CONNECT from " << session_id << ". Replying with existing token " << existing_token << std::endl;

        // Pick spawn position (for sending in accept)
        auto [spawn_x, spawn_y] = choose_spawn_position();
        
        // send accept again to ensure client receives it
        send_server_accept(session_id, existing_token, spawn_x, spawn_y);
        // update all clients with the current player list
        if (udp_server_) udp_server_->broadcast_player_list();
        return;
    }

    // Assign token but DON'T spawn entity yet (we're in lobby)
    uint32_t token = ecs_.next_session_token_++;
    ecs_.session_token_map_[session_id] = token;

    // Authenticate session and set player info
    if (udp_server_) {
        auto session = udp_server_->get_session(session_id);
        if (session) {
            session->set_authenticated(true);
            session->set_player_id(static_cast<int>(token));
            if (cc && cc->player_name[0] != '\0') {
                session->set_player_name(std::string(cc->player_name));
            }
        }
    }

    // Pick spawn position for later use
    auto [spawn_x, spawn_y] = choose_spawn_position();

    // reply and broadcast: send SERVER_ACCEPT then broadcast the CLIENT_LIST to all clients
    send_server_accept(session_id, token, spawn_x, spawn_y);
    // Do NOT spawn player entities on clients yet (we're in lobby) - instead broadcast the client list
    if (udp_server_) {
        udp_server_->send_player_list_to_client(session_id);
        udp_server_->broadcast_player_list();
    }

    std::cout << "Client " << session_id << " connected and authenticated with token " << token << " (entity will spawn on game start)" << std::endl;
}

std::pair<float,float> Multiplayer::choose_spawn_position() {
    auto &registry = ecs_.GetRegistry();
    std::vector<std::pair<float,float>> occupied;
    auto *positions = registry.get_if<position>();
    for (const auto &kv : ecs_.session_entity_map_) {
        entity ent = kv.second;
        if (positions && positions->has(ent)) {
            auto &p = (*positions)[ent];
            occupied.emplace_back(p.x, p.y);
        }
    }
    float base_x = 100.0f;
    float base_y = 100.0f;
    float spacing = 150.0f;
    float min_dist = 80.0f;
    for (int i = 0; i < 32; ++i) {
        float cx = base_x + i * spacing;
        int row = (i / 8);
        float cy = base_y + row * spacing * 0.5f;
        bool ok = true;
        for (auto &op : occupied) {
            float dx = op.first - cx;
            float dy = op.second - cy;
            if ((dx*dx + dy*dy) < (min_dist * min_dist)) { ok = false; break; }
        }
        if (ok) return {cx, cy};
    }
    return {base_x, base_y};
}

entity Multiplayer::spawn_player_entity(float x, float y) {
    auto &registry = ecs_.GetRegistry();
    entity ent = registry.spawn_entity();
    if (ecs_.get_factory()) {
        ecs_.get_factory()->create_component<position>(registry, ent, x, y);
        ecs_.get_factory()->create_component<velocity>(registry, ent, 0.0f, 0.0f);
        ecs_.get_factory()->create_component<InputBuffer>(registry, ent);
        ecs_.get_factory()->create_component<animation>(registry, ent, std::string(RTYPE_PATH_ASSETS) + "dedsec_eyeball-Sheet.png", 400.0f, 400.0f, 0.25f, 0.25f, 0, true);
    } else {
        std::cout << "Component factory not available; created entity without components" << std::endl;
    }
    return ent;
}

void Multiplayer::send_server_accept(const std::string &session_id, uint32_t token, float x, float y) {
    if (!ecs_.send_callback_) return;
    RType::Protocol::ServerAccept sa{};
    sa.player_id = token;
    sa.session_id = token;
    sa.spawn_x = x;
    sa.spawn_y = y;
    auto packet = RType::Protocol::create_packet(static_cast<uint8_t>(RType::Protocol::SystemMessage::SERVER_ACCEPT), sa, RType::Protocol::PacketFlags::RELIABLE);
    ecs_.send_callback_(session_id, packet);
}

void Multiplayer::broadcast_new_player_spawn(const std::string &session_id, uint32_t token, entity player_ent, float x, float y) {
    if (!ecs_.send_callback_) return;
    RType::Protocol::PlayerSpawn ps{};
    ps.player_token = token;
    ps.server_entity = static_cast<uint32_t>(player_ent);
    ps.x = x; ps.y = y; ps.health = 1.0f;
    auto player_packet = RType::Protocol::create_packet(static_cast<uint8_t>(RType::Protocol::GameMessage::PLAYER_SPAWN), ps, RType::Protocol::PacketFlags::RELIABLE);

    RType::Protocol::PlayerRemoteSpawn es{};
    es.player_token = token;
    es.server_entity = static_cast<uint32_t>(player_ent);
    es.x = x; es.y = y; es.health = 1.0f;
    auto remote_packet = RType::Protocol::create_packet(static_cast<uint8_t>(RType::Protocol::GameMessage::PLAYER_SPAWN_REMOTE), es, RType::Protocol::PacketFlags::NONE);

    for (const auto &kv : ecs_.session_token_map_) {
        const auto &s = kv.first;
        if (s == session_id) continue;
        ecs_.send_callback_(s, remote_packet);
    }

    // send player spawn to the owner
    ecs_.send_callback_(session_id, player_packet);
}

void Multiplayer::handle_client_disconnect(const std::string &session_id, const std::vector<char> &payload) {
    uint32_t leaving_player = 0;
    bool found_player = false;
    std::string leaving_session;

    if (payload.size() >= sizeof(RType::Protocol::ClientDisconnect)) {
        RType::Protocol::ClientDisconnect cd;
        memcpy(&cd, payload.data(), sizeof(cd));
        leaving_player = cd.player_id;
        found_player = true;
        // find session by token
        for (const auto &kv : ecs_.session_token_map_) {
            if (kv.second == leaving_player) { leaving_session = kv.first; break; }
        }
        std::cout << "ClientDisconnect payload: player_id=" << leaving_player << std::endl;
    } else {
        auto sit = ecs_.session_token_map_.find(session_id);
        if (sit != ecs_.session_token_map_.end()) {
            leaving_player = sit->second;
            found_player = true;
            leaving_session = session_id;
            std::cout << "Session " << session_id << " disconnected, inferred token " << leaving_player << std::endl;
        } else {
            std::cout << "Session " << session_id << " disconnected but no token found" << std::endl;
        }
    }

    if (found_player) {
        if (ecs_.send_callback_) {
            RType::Protocol::PlayerInfo pi{}; pi.player_id = leaving_player; pi.ready_state = 0; strncpy(pi.name, "", sizeof(pi.name)-1); pi.name[sizeof(pi.name)-1] = '\0';
            auto leave_packet = RType::Protocol::create_packet(static_cast<uint8_t>(RType::Protocol::GameMessage::PLAYER_LEAVE), pi, RType::Protocol::PacketFlags::NONE);
            for (const auto &kv : ecs_.session_token_map_) { const auto &s = kv.first; if (s == leaving_session) continue; ecs_.send_callback_(s, leave_packet); }
        }

        if (!leaving_session.empty()) {
            auto it = ecs_.session_entity_map_.find(leaving_session);
            if (it != ecs_.session_entity_map_.end()) {
                ecs_.GetRegistry().kill_entity(it->second);
                ecs_.session_entity_map_.erase(it);
            }
            ecs_.session_token_map_.erase(leaving_session);

            // Disconnect the session in the UdpServer so it's properly removed
            if (udp_server_) {
                auto session = udp_server_->get_session(leaving_session);
                if (session) {
                    std::cout << "Disconnecting session from UdpServer" << std::endl;
                    session->disconnect();
                }
            }
        }
    }
}

void Multiplayer::handle_player_input(const std::string &session_id, const std::vector<char> &payload) {
    using RType::Protocol::PlayerInput;
    using RType::Protocol::InputFlags;
    using RType::Protocol::PositionUpdate;
    using RType::Protocol::GameMessage;

    if (payload.size() < sizeof(PlayerInput)) {
        return;
    }

    PlayerInput input;
    memcpy(&input, payload.data(), sizeof(PlayerInput));

    // Find the player entity for this session
    auto it = ecs_.session_entity_map_.find(session_id);
    if (it == ecs_.session_entity_map_.end()) {
        return;
    }

    entity player_ent = it->second;
    auto& registry = ecs_.GetRegistry();

    // Apply velocity based on input state
    float speed = 300.0f; // Player movement speed
    float vx = 0.0f, vy = 0.0f;

    if (input.input_state & static_cast<uint8_t>(InputFlags::UP))    vy -= speed;
    if (input.input_state & static_cast<uint8_t>(InputFlags::DOWN))  vy += speed;
    if (input.input_state & static_cast<uint8_t>(InputFlags::LEFT))  vx -= speed;
    if (input.input_state & static_cast<uint8_t>(InputFlags::RIGHT)) vx += speed;

    // Update or create velocity component
    auto* vel_arr = registry.get_if<velocity>();
    if (vel_arr && vel_arr->has(player_ent)) {
        (*vel_arr)[player_ent] = velocity(vx, vy);
    }
}

void Multiplayer::handle_game_message(const std::string &session_id, uint8_t msg_type, const std::vector<char> &payload) {
    // route input to an existing session entity's InputBuffer
    auto it = ecs_.session_entity_map_.find(session_id);
    if (it != ecs_.session_entity_map_.end()) {
        entity player_ent = it->second;
        ecs_.GetRegistry().register_component<InputBuffer>();
        auto &registry = ecs_.GetRegistry();
        auto* buffers = registry.get_if<InputBuffer>();
        // make a copy of payload to construct Input
        std::vector<char> payload_copy = payload;
        uint64_t timestamp = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count());
        if (buffers && buffers->has(player_ent)) {
            auto& buf = (*buffers)[player_ent];
            Input input{msg_type, std::move(payload_copy), timestamp};
            buf.inputs.push_back(std::move(input));
        } else {
            InputBuffer ib;
            Input input{msg_type, std::move(payload_copy), timestamp};
            ib.inputs.push_back(std::move(input));
            if (ecs_.get_factory()) ecs_.get_factory()->create_component<InputBuffer>(ecs_.GetRegistry(), player_ent, std::move(ib));
            else std::cout << "Cannot create InputBuffer: factory missing" << std::endl;
        }
    } else {
        std::cout << "No mapped entity for session " << session_id << ". Packet ignored or handled elsewhere." << std::endl;
    }
}

void Multiplayer::handle_client_ready(const std::string &session_id, const std::vector<char> &payload) {
    using RType::Protocol::ClientReady;

    if (payload.size() < sizeof(ClientReady)) {
        std::cerr << "Invalid CLIENT_READY payload size" << std::endl;
        return;
    }

    ClientReady cr;
    memcpy(&cr, payload.data(), sizeof(ClientReady));

    // Update session ready state
    if (udp_server_) {
        auto session = udp_server_->get_session(session_id);
        if (session) {
            session->set_ready(cr.ready_state != 0);
            // Broadcast updated player list to all clients
            udp_server_->broadcast_player_list();
            std::cout << "Broadcasted updated player list" << std::endl;

            // Check if all players are ready and start game if conditions met
            udp_server_->check_all_players_ready();
        } else {
            std::cerr << "Session " << session_id << " not found" << std::endl;
        }
    }
}

void Multiplayer::handle_client_unready(const std::string &session_id, const std::vector<char> &payload) {
    using RType::Protocol::ClientReady;

    if (payload.size() < sizeof(ClientReady)) {
        std::cerr << "Invalid CLIENT_UNREADY payload size" << std::endl;
        return;
    }

    ClientReady cr;
    memcpy(&cr, payload.data(), sizeof(ClientReady));
    // Update session ready state to false
    if (udp_server_) {
        auto session = udp_server_->get_session(session_id);
        if (session) {
            session->set_ready(false);
            // Broadcast updated player list to all clients
            udp_server_->broadcast_player_list();
            std::cout << "Broadcasted updated player list" << std::endl;
        } else {
            std::cerr << "Session " << session_id << " not found" << std::endl;
        }
    }
}

void Multiplayer::spawn_all_players() {
    std::cout << "Spawning all connected players for game start..." << std::endl;

    // Iterate through all connected sessions and spawn their entities + broadcast
    for (const auto &kv : ecs_.session_token_map_) {
        const std::string& session_id = kv.first;
        uint32_t token = kv.second;

        // Check if entity already exists (it shouldn't in the new flow)
        auto entity_it = ecs_.session_entity_map_.find(session_id);
        if (entity_it != ecs_.session_entity_map_.end()) {
            std::cout << "WARNING: Player entity already exists for session " 
                      << session_id << " (token " << token << "). Using existing entity." << std::endl;
            
            entity player_ent = entity_it->second;
            
            // Get position from entity
            float x = 100.0f;
            float y = 100.0f;
            try {
                auto& positions = ecs_.registry_.get_components<position>();
                const auto& pos = positions[player_ent];
                x = pos.x;
                y = pos.y;
            } catch (...) {
                // If position doesn't exist, use default
            }

            std::cout << "Broadcasting spawn for player " 
                      << token << " at (" << x << ", " << y << ")" << std::endl;
            broadcast_new_player_spawn(session_id, token, player_ent, x, y);
            continue;
        }

        // Spawn new entity for this player
        auto [spawn_x, spawn_y] = choose_spawn_position();
        entity player_ent = spawn_player_entity(spawn_x, spawn_y);
        ecs_.session_entity_map_[session_id] = player_ent;

        std::cout << "Spawned and broadcasting player " 
                  << token << " (entity " << player_ent << ") at (" << spawn_x << ", " << spawn_y << ")" << std::endl;
        broadcast_new_player_spawn(session_id, token, player_ent, spawn_x, spawn_y);
    }

    std::cout << "All players spawned!" << std::endl;
}

void Multiplayer::broadcast_positions() {
    using RType::Protocol::PositionUpdate;
    using RType::Protocol::GameMessage;

    if (!udp_server_) return;

    auto& registry = ecs_.GetRegistry();
    auto* pos_arr = registry.get_if<position>();
    auto* vel_arr = registry.get_if<velocity>();

    if (!pos_arr || !vel_arr) return;

    // Iterate through all session entities (players)
    for (const auto& [session_id, player_ent] : ecs_.session_entity_map_) {
        // Get the player token for this session
        auto token_it = ecs_.session_token_map_.find(session_id);
        if (token_it == ecs_.session_token_map_.end()) continue;

        uint32_t player_token = token_it->second;

        // Check if entity has position and velocity components
        if (!pos_arr->has(player_ent)) continue;

        auto& pos = (*pos_arr)[player_ent];

        float vx = 0.0f, vy = 0.0f;
        if (vel_arr->has(player_ent)) {
            auto& vel = (*vel_arr)[player_ent];
            vx = vel.vx;
            vy = vel.vy;
        }

        // Create position update message
        PositionUpdate pos_update;
        pos_update.entity_id = player_token;
        pos_update.x = pos.x;
        pos_update.y = pos.y;
        pos_update.vx = vx;
        pos_update.vy = vy;
        pos_update.timestamp = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());

        // Create properly formatted packet
        auto packet = RType::Protocol::create_packet(
            static_cast<uint8_t>(GameMessage::POSITION_UPDATE),
            pos_update
        );

        // Broadcast to all clients
        udp_server_->broadcast(reinterpret_cast<const char*>(packet.data()), packet.size());
    }
}

void Multiplayer::broadcast_enemy_spawn(entity ent, uint8_t enemy_type, float x, float y) {
    if (!ecs_.send_callback_) return;
    if (!udp_server_) return;
    
    RType::Protocol::EntityCreate ec{};
    ec.entity_id = static_cast<uint32_t>(ent);
    ec.entity_type = enemy_type;
    ec.x = x;
    ec.y = y;
    ec.health = 15.0f;
    
    auto packet = RType::Protocol::create_packet(
        static_cast<uint8_t>(RType::Protocol::GameMessage::ENTITY_CREATE),
        ec,
        RType::Protocol::PacketFlags::RELIABLE
    );
    
    // Broadcast to all connected clients
    udp_server_->broadcast(reinterpret_cast<const char*>(packet.data()), packet.size());
    
    std::cout << "[Multiplayer] " << "Broadcasted enemy spawn: entity=" << ent 
              << " type=" << (int)enemy_type << " pos=(" << x << ", " << y << ")" << std::endl;
}

} // namespace RType::Network
