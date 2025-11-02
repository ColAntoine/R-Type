#include "Multiplayer.hpp"
#include "ServerECS/ServerECS.hpp"
#include "ServerECS/Communication/Lobby.hpp"
#include "ServerECS/Communication/InGame.hpp"
#include "Network/UDPServer.hpp"
#include "Network/Session.hpp"
#include <iostream>

namespace RType::Network {

Multiplayer::Multiplayer(ServerECS &ecs, int maxLobbies, int maxPlayers) : ecs_(ecs), max_lobbies_(maxLobbies), max_players_(maxPlayers) {
    lobby_ = std::make_unique<Lobby>(ecs_);
    ingame_ = std::make_unique<InGame>(ecs_);
}

Multiplayer::~Multiplayer() = default;

void Multiplayer::set_udp_server(UdpServer* server) {
    udp_server_ = server;
    if (lobby_) lobby_->set_udp_server(server);
    if (ingame_) ingame_->set_udp_server(server);

    // Register game start callback to spawn all players when game starts
    if (udp_server_) {
        udp_server_->set_game_start_callback([this]() {
            if (ingame_) ingame_->spawn_all_players();
        });
    }
}

void Multiplayer::handle_packet(const std::string &session_id, const std::vector<char> &data) {
    if (data.empty()) return;
    uint8_t msg_type = static_cast<uint8_t>(data[0]);
    std::vector<char> payload;
    if (data.size() > 1) payload.insert(payload.end(), data.begin() + 1, data.end());

    using RType::Protocol::SystemMessage;

    // Route to lobby or in-game handlers depending on current server state
    if (!ecs_.is_game_started()) {
        // Lobby-phase messages
        if (msg_type == static_cast<uint8_t>(SystemMessage::CLIENT_CONNECT)) {
            if (lobby_) lobby_->handle_client_connect(session_id, payload);
            return;
        }
        if (msg_type == static_cast<uint8_t>(SystemMessage::CLIENT_DISCONNECT)) {
            if (lobby_) lobby_->handle_client_disconnect(session_id, payload);
            return;
        }
        if (msg_type == static_cast<uint8_t>(SystemMessage::CLIENT_READY)) {
            if (lobby_) lobby_->handle_client_ready(session_id, payload);
            return;
        }
        if (msg_type == static_cast<uint8_t>(SystemMessage::CLIENT_UNREADY)) {
            if (lobby_) lobby_->handle_client_unready(session_id, payload);
            return;
        }
        // Handle instance requests during lobby phase (clients ask front server to spawn instances)
        if (msg_type == static_cast<uint8_t>(SystemMessage::REQUEST_INSTANCE)) {
            std::cout << "[Multiplayer] REQUEST_INSTANCE (lobby) from " << session_id << std::endl;
            if (ecs_.instance_request_cb_) {
                ecs_.instance_request_cb_(session_id);
            } else {
                std::cout << "[Multiplayer] Instance requests not supported on this server" << std::endl;
            }
            return;
        }
        // Other messages during lobby are ignored or queued by ServerECS
        return;
    }

    // Game-phase routing
    using RType::Protocol::GameMessage;
    if (msg_type == static_cast<uint8_t>(SystemMessage::CLIENT_DISCONNECT)) {
        if (ingame_) lobby_->handle_client_disconnect(session_id, payload);
        return;
    }
    if (msg_type == static_cast<uint8_t>(SystemMessage::CLIENT_READY)) {
        lobby_->handle_client_ready(session_id, payload);
        return;
    }
    if (msg_type == static_cast<uint8_t>(SystemMessage::CLIENT_UNREADY)) {
        lobby_->handle_client_unready(session_id, payload);
        return;
    }

    if (msg_type == static_cast<uint8_t>(SystemMessage::REQUEST_INSTANCE)) {
        // Client asks the front server to create a new instance (lobby+game)
        std::cout << "[Multiplayer] REQUEST_INSTANCE from " << session_id << std::endl;
        if (ecs_.instance_request_cb_) {
            ecs_.instance_request_cb_(session_id);
        } else {
            std::cout << "[Multiplayer] Instance requests not supported on this server" << std::endl;
        }
        return;
    }

    // Check for PLAYER_INPUT game message
    using RType::Protocol::GameMessage;
    if (msg_type == static_cast<uint8_t>(GameMessage::PLAYER_INPUT)) {
        if (ingame_) ingame_->handle_player_input(session_id, payload);
        return;
    }
    if (msg_type == static_cast<uint8_t>(GameMessage::PLAYER_SHOOT)) {
        if (ingame_) ingame_->handle_player_shoot(session_id, payload);
        return;
    }
    if (msg_type == static_cast<uint8_t>(GameMessage::PLAYER_UNSHOOT)) {
        if (ingame_) ingame_->handle_player_unshoot(session_id, payload);
        return;
    }

    // Otherwise forward to general in-game message handler
    if (ingame_) ingame_->handle_game_message(session_id, msg_type, payload);
}

void Multiplayer::spawn_all_players() {
    if (ingame_) ingame_->spawn_all_players();
}

void Multiplayer::broadcast_loop() {
    // InGame now exposes a split broadcast loop (broadcast_loop) that calls
    // per-player position and shoot helpers. Call that instead.
    if (ingame_) ingame_->broadcast_loop();
}

void Multiplayer::broadcast_enemy_spawn(entity ent, uint8_t enemy_type, float x, float y) {
    if (ingame_) ingame_->broadcast_enemy_spawn(ent, enemy_type, x, y);
}

void Multiplayer::broadcast_entity_destroy(entity ent, uint8_t reason) {
    if (ingame_) ingame_->broadcast_entity_destroy(ent, reason);
}

} // namespace RType::Network
