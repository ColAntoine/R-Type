#include "Multiplayer.hpp"
#include "ServerECS/ServerECS.hpp"
#include "ServerECS/Communication/Lobby.hpp"
#include "ServerECS/Communication/InGame.hpp"
#include <iostream>
#include "ECS/Utils/Console.hpp"

namespace RType::Network {

Multiplayer::Multiplayer(ServerECS &ecs) : ecs_(ecs) {
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

    std::cout << Console::blue("[Multiplayer] ") << "Pkt type=" << int(msg_type) << " from " << session_id
              << " payload=" << payload.size() << std::endl;

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
        // Other messages during lobby are ignored or queued by ServerECS
        return;
    }

    // Game-phase routing
    using RType::Protocol::GameMessage;
    if (msg_type == static_cast<uint8_t>(SystemMessage::CLIENT_DISCONNECT)) {
        if (ingame_) ingame_->handle_client_disconnect(session_id, payload);
        return;
    }
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

} // namespace RType::Network
