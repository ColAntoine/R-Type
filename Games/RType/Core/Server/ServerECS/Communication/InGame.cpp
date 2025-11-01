#include "InGame.hpp"
#include "ServerECS/ServerECS.hpp"
#include "Network/UDPServer.hpp"
#include "Network/Session.hpp"
#include "ECS/Components/InputBuffer.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/Components/Collider.hpp"
#include "ECS/Components/Animation.hpp"
#include "Entity/Components/Weapon/Weapon.hpp"
#include "Entity/Components/Player/Player.hpp"
#include "Constants.hpp"
#include <iostream>
#include <chrono>

namespace RType::Network {

InGame::InGame(ServerECS &ecs) : ecs_(ecs) {}
InGame::~InGame() = default;

void InGame::set_udp_server(UdpServer* server) {
    udp_server_ = server;
}

void InGame::handle_client_disconnect(const std::string &session_id, const std::vector<char> &payload) {
    // Reuse the same logic as lobby when disconnect occurs during game
    uint32_t leaving_player = 0;
    bool found_player = false;
    std::string leaving_session;

    if (payload.size() >= sizeof(RType::Protocol::ClientDisconnect)) {
        RType::Protocol::ClientDisconnect cd;
        memcpy(&cd, payload.data(), sizeof(cd));
        leaving_player = cd.player_id;
        found_player = true;
        for (const auto &kv : ecs_.session_token_map_) {
            if (kv.second == leaving_player) { leaving_session = kv.first; break; }
        }
        std::cout << "[InGame] ClientDisconnect payload: player_id=" << leaving_player << std::endl;
    } else {
        auto sit = ecs_.session_token_map_.find(session_id);
        if (sit != ecs_.session_token_map_.end()) {
            leaving_player = sit->second;
            found_player = true;
            leaving_session = session_id;
            std::cout << "[InGame] Session " << session_id << " disconnected, inferred token " << leaving_player << std::endl;
        } else {
            std::cout << "[InGame] Session " << session_id << " disconnected but no token found" << std::endl;
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

            if (udp_server_) {
                auto session = udp_server_->get_session(leaving_session);
                if (session) {
                    std::cout << "[InGame] Disconnecting session from UdpServer" << std::endl;
                    session->disconnect();
                }
            }
        }
    }
}

void InGame::handle_player_input(const std::string &session_id, const std::vector<char> &payload) {
    using RType::Protocol::PlayerInput;
    using RType::Protocol::InputFlags;
    using RType::Protocol::PositionUpdate;
    using RType::Protocol::GameMessage;

    if (payload.size() < sizeof(PlayerInput)) {
        return;
    }

    PlayerInput input;
    memcpy(&input, payload.data(), sizeof(PlayerInput));

    auto it = ecs_.session_entity_map_.find(session_id);
    if (it == ecs_.session_entity_map_.end()) {
        return;
    }

    entity player_ent = it->second;
    auto& registry = ecs_.GetRegistry();

    float speed = 300.0f;
    float vx = 0.0f, vy = 0.0f;

    if (input.input_state & static_cast<uint8_t>(InputFlags::UP))    vy -= speed;
    if (input.input_state & static_cast<uint8_t>(InputFlags::DOWN))  vy += speed;
    if (input.input_state & static_cast<uint8_t>(InputFlags::LEFT))  vx -= speed;
    if (input.input_state & static_cast<uint8_t>(InputFlags::RIGHT)) vx += speed;

    auto* vel_arr = registry.get_if<velocity>();
    if (vel_arr && vel_arr->has(player_ent)) {
        (*vel_arr)[player_ent] = velocity(vx, vy);
    }
}

void InGame::handle_player_shoot(const std::string &session_id, const std::vector<char> &payload) {
    using RType::Protocol::PlayerShoot;
    using RType::Protocol::GameMessage;

    is_shooting_[session_id] = true;

    // Build PlayerShoot to broadcast to other clients. Prefer payload from sender if present.
    PlayerShoot ps{};
    if (payload.size() >= sizeof(PlayerShoot)) {
        memcpy(&ps, payload.data(), sizeof(PlayerShoot));
    } else {
        // Fill minimal info: player id from session token map
        auto tit = ecs_.session_token_map_.find(session_id);
        if (tit != ecs_.session_token_map_.end()) ps.player_id = tit->second;
        else ps.player_id = 0;
        ps.start_x = 0.0f; ps.start_y = 0.0f; ps.dir_x = 1.0f; ps.dir_y = 0.0f; ps.weapon_type = 0;
    }

    // Broadcast to all sessions except the shooter
    if (ecs_.send_callback_) {
        auto packet = RType::Protocol::create_packet(static_cast<uint8_t>(GameMessage::PLAYER_SHOOT), ps, RType::Protocol::PacketFlags::NONE);
        for (const auto &kv : ecs_.session_token_map_) {
            const auto &s = kv.first;
            if (s == session_id) continue;
            ecs_.send_callback_(s, packet);
        }
    }
}

void InGame::handle_player_unshoot(const std::string &session_id, const std::vector<char> &payload) {
    using RType::Protocol::PlayerShoot;
    using RType::Protocol::GameMessage;

    is_shooting_[session_id] = false;

    // Build PlayerShoot payload (reuse PlayerShoot structure) to notify others this player stopped shooting.
    PlayerShoot ps{};
    if (payload.size() >= sizeof(PlayerShoot)) {
        memcpy(&ps, payload.data(), sizeof(PlayerShoot));
    } else {
        auto tit = ecs_.session_token_map_.find(session_id);
        if (tit != ecs_.session_token_map_.end()) ps.player_id = tit->second;
        else ps.player_id = 0;
        ps.start_x = 0.0f; ps.start_y = 0.0f; ps.dir_x = 1.0f; ps.dir_y = 0.0f; ps.weapon_type = 0;
    }

    if (ecs_.send_callback_) {
        auto packet = RType::Protocol::create_packet(static_cast<uint8_t>(GameMessage::PLAYER_UNSHOOT), ps, RType::Protocol::PacketFlags::NONE);
        for (const auto &kv : ecs_.session_token_map_) {
            const auto &s = kv.first;
            if (s == session_id) continue;
            ecs_.send_callback_(s, packet);
        }
    }

}

void InGame::handle_game_message(const std::string &session_id, uint8_t msg_type, const std::vector<char> &payload) {
    auto it = ecs_.session_entity_map_.find(session_id);
    if (it != ecs_.session_entity_map_.end()) {
        entity player_ent = it->second;
        auto &registry = ecs_.GetRegistry();
        auto* buffers = registry.get_if<InputBuffer>();
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
            else std::cout << "[InGame] Cannot create InputBuffer: factory missing" << std::endl;
        }
    } else {
        std::cout << "[InGame] No mapped entity for session " << session_id << ". Packet ignored or handled elsewhere." << std::endl;
    }
}

void InGame::spawn_all_players() {
    std::cout << "[InGame] Spawning all connected players for game start..." << std::endl;
    // Arrange players vertically with a fixed spacing to avoid overlapping spawns.
    const float base_x = 100.0f;
    const float base_y = 100.0f;
    const float spacing_y = 80.0f; // pixels between players vertically
    size_t idx = 0;

    for (const auto &kv : ecs_.session_token_map_) {
        const std::string& session_id = kv.first;
        uint32_t token = kv.second;

        // Compute spawn position based on index so each player is placed under the previous one
        float spawn_x = base_x;
        float spawn_y = base_y + static_cast<float>(idx) * spacing_y;

        auto entity_it = ecs_.session_entity_map_.find(session_id);
        if (entity_it != ecs_.session_entity_map_.end()) {
            std::cout << "[InGame] WARNING: Player entity already exists for session "
                      << session_id << " (token " << token << "). Repositioning and using existing entity." << std::endl;

            entity player_ent = entity_it->second;

            // Ensure the entity position matches the computed spawn location so clients see them separated
            auto& registry = ecs_.GetRegistry();
            auto* pos_arr = registry.get_if<position>();
            if (pos_arr) {
                if (pos_arr->has(player_ent)) (*pos_arr)[player_ent] = position{spawn_x, spawn_y};
                else registry.emplace_component<position>(player_ent, spawn_x, spawn_y);
            } else {
                if (ecs_.get_factory()) ecs_.get_factory()->create_component<position>(registry, player_ent, spawn_x, spawn_y);
            }

            std::cout << "[InGame] Broadcasting spawn for player " << token << " at (" << spawn_x << ", " << spawn_y << ")" << std::endl;
            // create and send packets
            if (ecs_.send_callback_) {
                RType::Protocol::PlayerSpawn ps{};
                ps.player_token = token;
                ps.server_entity = static_cast<uint32_t>(player_ent);
                ps.x = spawn_x; ps.y = spawn_y; ps.health = 1.0f;
                auto player_packet = RType::Protocol::create_packet(static_cast<uint8_t>(RType::Protocol::GameMessage::PLAYER_SPAWN), ps, RType::Protocol::PacketFlags::RELIABLE);
                RType::Protocol::PlayerRemoteSpawn es{};
                es.player_token = token;
                es.server_entity = static_cast<uint32_t>(player_ent);
                es.x = spawn_x; es.y = spawn_y; es.health = 1.0f;
                auto remote_packet = RType::Protocol::create_packet(static_cast<uint8_t>(RType::Protocol::GameMessage::PLAYER_SPAWN_REMOTE), es, RType::Protocol::PacketFlags::NONE);
                for (const auto &kv2 : ecs_.session_token_map_) {
                    const auto &s = kv2.first;
                    if (s == session_id) continue;
                    ecs_.send_callback_(s, remote_packet);
                }
                ecs_.send_callback_(session_id, player_packet);
            }
            // increment index for next player
            ++idx;
            continue;
        }

        // New player: spawn entity and attach needed components with computed spawn pos
        entity player_ent = ecs_.GetRegistry().spawn_entity();
        if (ecs_.get_factory()) {
            ecs_.get_factory()->create_component<position>(ecs_.GetRegistry(), player_ent, spawn_x, spawn_y);
            ecs_.get_factory()->create_component<velocity>(ecs_.GetRegistry(), player_ent, 0.0f, 0.0f);
            ecs_.get_factory()->create_component<InputBuffer>(ecs_.GetRegistry(), player_ent);
            ecs_.get_factory()->create_component<animation>(ecs_.GetRegistry(), player_ent, std::string(RTYPE_PATH_ASSETS) + "dedsec_eyeball-Sheet.png", 400.0f, 400.0f, 0.25f, 0.25f, 0, true);
            ecs_.get_factory()->create_component<collider>(ecs_.GetRegistry(), player_ent, COLLISION_WIDTH, COLLISION_HEIGHT, -COLLISION_WIDTH/2, -COLLISION_HEIGHT/2);
            ecs_.get_factory()->create_component<Weapon>(ecs_.GetRegistry(), player_ent);
            ecs_.get_factory()->create_component<Health>(ecs_.GetRegistry(), player_ent);
            ecs_.get_factory()->create_component<Player>(ecs_.GetRegistry(), player_ent);
        }
        ecs_.session_entity_map_[session_id] = player_ent;

        std::cout << "[InGame] Spawned and broadcasting player " << token << " (entity " << player_ent << ") at (" << spawn_x << ", " << spawn_y << ")" << std::endl;

        if (ecs_.send_callback_) {
            RType::Protocol::PlayerSpawn ps{};
            ps.player_token = token;
            ps.server_entity = static_cast<uint32_t>(player_ent);
            ps.x = spawn_x; ps.y = spawn_y; ps.health = 1.0f;
            auto player_packet = RType::Protocol::create_packet(static_cast<uint8_t>(RType::Protocol::GameMessage::PLAYER_SPAWN), ps, RType::Protocol::PacketFlags::RELIABLE);
            RType::Protocol::PlayerRemoteSpawn es{};
            es.player_token = token;
            es.server_entity = static_cast<uint32_t>(player_ent);
            es.x = spawn_x; es.y = spawn_y; es.health = 1.0f;
            auto remote_packet = RType::Protocol::create_packet(static_cast<uint8_t>(RType::Protocol::GameMessage::PLAYER_SPAWN_REMOTE), es, RType::Protocol::PacketFlags::NONE);
            for (const auto &kv2 : ecs_.session_token_map_) {
                const auto &s = kv2.first;
                if (s == session_id) continue;
                ecs_.send_callback_(s, remote_packet);
            }
            ecs_.send_callback_(session_id, player_packet);
        }
        ++idx;
    }

    std::cout << "[InGame] All players spawned!" << std::endl;
}

void InGame::broadcast_loop()
{
    if (!udp_server_) return;

    auto& registry = ecs_.GetRegistry();
    auto* pos_arr = registry.get_if<position>();
    auto* vel_arr = registry.get_if<velocity>();
    if (!pos_arr || !vel_arr) return;

    for (const auto& [session_id, player_ent] : ecs_.session_entity_map_) {
        broadcast_shoot(session_id, player_ent);
        broadcast_positions(session_id, player_ent, vel_arr, pos_arr);
    }
}

void InGame::broadcast_positions(const std::string& session_id, const entity& player_ent,
                                sparse_set<velocity, std::allocator<velocity>> *vel_arr,
                                sparse_set<position, std::allocator<position>> *pos_arr) {
    using RType::Protocol::PositionUpdate;
    using RType::Protocol::GameMessage;
    auto token_it = ecs_.session_token_map_.find(session_id);
    if (token_it == ecs_.session_token_map_.end()) return;

    uint32_t player_token = token_it->second;

    if (!pos_arr->has(player_ent)) return;

    auto& pos = (*pos_arr)[player_ent];

    float vx = 0.0f, vy = 0.0f;
    if (vel_arr->has(player_ent)) {
        auto& vel = (*vel_arr)[player_ent];
        vx = vel.vx;
        vy = vel.vy;
    }

    PositionUpdate pos_update;
    pos_update.entity_id = player_token;
    pos_update.x = pos.x;
    pos_update.y = pos.y;
    pos_update.vx = vx;
    pos_update.vy = vy;
    pos_update.timestamp = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());

    auto packet = RType::Protocol::create_packet(static_cast<uint8_t>(GameMessage::POSITION_UPDATE), pos_update);
    udp_server_->broadcast(reinterpret_cast<const char*>(packet.data()), packet.size());
}

void InGame::broadcast_shoot(const std::string& session_id, const entity& player_ent)
{
    auto sit_sh = is_shooting_.find(session_id);
    if (sit_sh != is_shooting_.end() && sit_sh->second) {
        auto &registry = ecs_.GetRegistry();
        auto* weapons = registry.get_if<Weapon>();
        if (weapons && weapons->has(player_ent)) {
            auto &w = (*weapons)[player_ent];
            w._wantsToFire = true;
            w._ownerId = static_cast<int>(player_ent);
        }
    }
}

void InGame::broadcast_enemy_spawn(entity ent, uint8_t enemy_type, float x, float y) {
    if (!ecs_.send_callback_) return;
    if (!udp_server_) return;

    RType::Protocol::EntityCreate ec{};
    ec.entity_id = static_cast<uint32_t>(ent);
    ec.entity_type = enemy_type;
    ec.x = x;
    ec.y = y;
    ec.health = 15.0f;

    auto packet = RType::Protocol::create_packet(static_cast<uint8_t>(RType::Protocol::GameMessage::ENTITY_CREATE), ec, RType::Protocol::PacketFlags::RELIABLE);
    udp_server_->broadcast(reinterpret_cast<const char*>(packet.data()), packet.size());

    std::cout << "[InGame] Broadcasted enemy spawn: entity=" << ent
              << " type=" << (int)enemy_type << " pos=(" << x << ", " << y << ")" << std::endl;
}

} // namespace RType::Network
