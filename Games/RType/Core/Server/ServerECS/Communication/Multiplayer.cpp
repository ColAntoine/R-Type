#include "Multiplayer.hpp"
#include "ServerECS/ServerECS.hpp"
#include <iostream>
#include "Utils/Console.hpp"
#include "ECS/Components/InputBuffer.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"

namespace RType::Network {

Multiplayer::Multiplayer(ServerECS &ecs) : ecs_(ecs) {}
Multiplayer::~Multiplayer() = default;

void Multiplayer::handle_packet(const std::string &session_id, const std::vector<char> &data) {
    if (data.empty()) return;
    uint8_t msg_type = static_cast<uint8_t>(data[0]);
    std::vector<char> payload;
    if (data.size() > 1) payload.insert(payload.end(), data.begin() + 1, data.end());

    std::cout << Console::blue("[Multiplayer] ") << "Pkt type=" << int(msg_type) << " from " << session_id
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

    // Otherwise treat as game message / input
    handle_game_message(session_id, msg_type, payload);
}

void Multiplayer::handle_client_connect(const std::string &session_id, const std::vector<char> &payload) {
    const RType::Protocol::ClientConnect* cc = nullptr;
    if (!payload.empty() && payload.size() >= sizeof(RType::Protocol::ClientConnect)) {
        cc = reinterpret_cast<const RType::Protocol::ClientConnect*>(payload.data());
    }

    // pick spawn position
    auto [spawn_x, spawn_y] = choose_spawn_position();

    // spawn entity server-side and register mappings
    entity player_ent = spawn_player_entity(spawn_x, spawn_y);
    ecs_.session_entity_map_[session_id] = player_ent;
    uint32_t token = ecs_.next_session_token_++;
    ecs_.session_token_map_[session_id] = token;

    // reply and broadcast
    send_server_accept(session_id, token, spawn_x, spawn_y);
    broadcast_new_player_spawn(session_id, token, player_ent, spawn_x, spawn_y);
    send_existing_players_to_newcomer(session_id);
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
    } else {
        std::cout << Console::yellow("[Multiplayer] ") << "Component factory not available; created entity without components" << std::endl;
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

void Multiplayer::send_existing_players_to_newcomer(const std::string &session_id) {
    if (!ecs_.send_callback_) return;
    for (const auto &kv : ecs_.session_entity_map_) {
        const auto &existing_session = kv.first;
        const auto &existing_ent = kv.second;
        if (existing_session == session_id) continue;

        RType::Protocol::PlayerRemoteSpawn existing_es{};
        auto tok_it = ecs_.session_token_map_.find(existing_session);
        if (tok_it != ecs_.session_token_map_.end()) existing_es.player_token = tok_it->second;
        else existing_es.player_token = static_cast<uint32_t>(existing_ent);
        auto &reg = ecs_.GetRegistry();
        auto *positions = reg.get_if<position>();
        if (positions && positions->has(existing_ent)) {
            auto &p = (*positions)[existing_ent];
            existing_es.x = p.x; existing_es.y = p.y;
        } else { existing_es.x = 100.0f; existing_es.y = 100.0f; }
        existing_es.server_entity = static_cast<uint32_t>(existing_ent);
        existing_es.health = 1.0f;

        auto existing_packet = RType::Protocol::create_packet(static_cast<uint8_t>(RType::Protocol::GameMessage::PLAYER_SPAWN_REMOTE), existing_es, RType::Protocol::PacketFlags::NONE);
        ecs_.send_callback_(session_id, existing_packet);
    }
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
        std::cout << Console::yellow("[Multiplayer] ") << "ClientDisconnect payload: player_id=" << leaving_player << std::endl;
    } else {
        auto sit = ecs_.session_token_map_.find(session_id);
        if (sit != ecs_.session_token_map_.end()) {
            leaving_player = sit->second;
            found_player = true;
            leaving_session = session_id;
            std::cout << Console::yellow("[Multiplayer] ") << "Session " << session_id << " disconnected, inferred token " << leaving_player << std::endl;
        } else {
            std::cout << Console::yellow("[Multiplayer] ") << "Session " << session_id << " disconnected but no token found" << std::endl;
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
        }
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
        if (buffers && buffers->has(player_ent)) {
            auto& buf = (*buffers)[player_ent];
            Input input{msg_type, std::move(payload_copy), static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count())};
            buf.inputs.push_back(std::move(input));
        } else {
            InputBuffer ib; Input input{msg_type, std::move(payload_copy), static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count())}; ib.inputs.push_back(std::move(input));
            if (ecs_.get_factory()) ecs_.get_factory()->create_component<InputBuffer>(ecs_.GetRegistry(), player_ent, std::move(ib));
            else std::cout << Console::yellow("[Multiplayer] ") << "Cannot create InputBuffer: factory missing" << std::endl;
        }
    } else {
        std::cout << Console::yellow("[Multiplayer] ") << "No mapped entity for session " << session_id << ". Packet ignored or handled elsewhere." << std::endl;
    }
}

} // namespace RType::Network
