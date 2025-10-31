#include "Lobby.hpp"
#include "ServerECS/ServerECS.hpp"
#include "Network/UDPServer.hpp"
#include "Network/Session.hpp"
#include "ECS/Components/Position.hpp"
#include <iostream>

namespace RType::Network {

Lobby::Lobby(ServerECS &ecs) : ecs_(ecs) {}
Lobby::~Lobby() = default;

void Lobby::set_udp_server(UdpServer* server) {
    udp_server_ = server;
}

void Lobby::handle_client_connect(const std::string &session_id, const std::vector<char> &payload) {
    const RType::Protocol::ClientConnect* cc = nullptr;
    if (!payload.empty() && payload.size() >= sizeof(RType::Protocol::ClientConnect)) {
        cc = reinterpret_cast<const RType::Protocol::ClientConnect*>(payload.data());
    }

    // If the game has already started, do not accept new clients
    if (ecs_.is_game_started()) {
        std::cout << "[Lobby] Rejecting CLIENT_CONNECT from " << session_id << " (game already started)" << std::endl;
        if (ecs_.send_callback_) {
            auto packet = RType::Protocol::create_simple_packet(static_cast<uint8_t>(RType::Protocol::SystemMessage::SERVER_DISCONNECT), RType::Protocol::PacketFlags::RELIABLE);
            ecs_.send_callback_(session_id, packet);
        }
        return;
    }

    // If this session already has a token, reply with existing token and return
    auto existing_tok_it = ecs_.session_token_map_.find(session_id);
    if (existing_tok_it != ecs_.session_token_map_.end()) {
        uint32_t existing_token = existing_tok_it->second;
        std::cout << "[Lobby] Duplicate CLIENT_CONNECT from " << session_id << ". Replying with existing token " << existing_token << std::endl;

        auto [spawn_x, spawn_y] = choose_spawn_position();
        send_server_accept(session_id, existing_token, spawn_x, spawn_y);
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
    if (udp_server_) {
        udp_server_->send_player_list_to_client(session_id);
        udp_server_->broadcast_player_list();
    }

    std::cout << "[Lobby] Client " << session_id << " connected and authenticated with token " << token << " (entity will spawn on game start)" << std::endl;
}

void Lobby::handle_client_disconnect(const std::string &session_id, const std::vector<char> &payload) {
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
        std::cout << "[Lobby] ClientDisconnect payload: player_id=" << leaving_player << std::endl;
    } else {
        auto sit = ecs_.session_token_map_.find(session_id);
        if (sit != ecs_.session_token_map_.end()) {
            leaving_player = sit->second;
            found_player = true;
            leaving_session = session_id;
            std::cout << "[Lobby] Session " << session_id << " disconnected, inferred token " << leaving_player << std::endl;
        } else {
            std::cout << "[Lobby] Session " << session_id << " disconnected but no token found" << std::endl;
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
                    std::cout << "[Lobby] Disconnecting session from UdpServer" << std::endl;
                    session->disconnect();
                }
            }
        }
    }
}

void Lobby::handle_client_ready(const std::string &session_id, const std::vector<char> &payload) {
    using RType::Protocol::ClientReady;

    if (payload.size() < sizeof(ClientReady)) {
        std::cerr << "[Lobby] Invalid CLIENT_READY payload size" << std::endl;
        return;
    }

    ClientReady cr;
    memcpy(&cr, payload.data(), sizeof(ClientReady));

    if (udp_server_) {
        auto session = udp_server_->get_session(session_id);
        if (session) {
            session->set_ready(cr.ready_state != 0);
            udp_server_->broadcast_player_list();
            std::cout << "[Lobby] Broadcasted updated player list" << std::endl;
            udp_server_->check_all_players_ready();
        } else {
            std::cerr << "[Lobby] Session " << session_id << " not found" << std::endl;
        }
    }
}

void Lobby::handle_client_unready(const std::string &session_id, const std::vector<char> &payload) {
    using RType::Protocol::ClientReady;

    if (payload.size() < sizeof(ClientReady)) {
        std::cerr << "[Lobby] Invalid CLIENT_UNREADY payload size" << std::endl;
        return;
    }

    ClientReady cr;
    memcpy(&cr, payload.data(), sizeof(ClientReady));
    if (udp_server_) {
        auto session = udp_server_->get_session(session_id);
        if (session) {
            session->set_ready(false);
            udp_server_->broadcast_player_list();
            std::cout << "[Lobby] Broadcasted updated player list" << std::endl;
        } else {
            std::cerr << "[Lobby] Session " << session_id << " not found" << std::endl;
        }
    }
}

std::pair<float,float> Lobby::choose_spawn_position() {
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

void Lobby::send_server_accept(const std::string &session_id, uint32_t token, float x, float y) {
    if (!ecs_.send_callback_) return;
    RType::Protocol::ServerAccept sa{};
    sa.player_id = token;
    sa.session_id = token;
    sa.spawn_x = x;
    sa.spawn_y = y;
    auto packet = RType::Protocol::create_packet(static_cast<uint8_t>(RType::Protocol::SystemMessage::SERVER_ACCEPT), sa, RType::Protocol::PacketFlags::RELIABLE);
    ecs_.send_callback_(session_id, packet);
}

} // namespace RType::Network
