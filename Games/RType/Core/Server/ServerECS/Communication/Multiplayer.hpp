#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

#include "Protocol/Protocol.hpp"
#include "ECS/Entity.hpp"
#include "Network/UDPServer.hpp"

namespace RType::Network {

class ServerECS; // forward

/**
 * Multiplayer - encapsulates network message handling for the server ECS.
 *
 * Responsibilities:
 *  - handle CLIENT_CONNECT / SERVER_ACCEPT and PLAYER_JOIN broadcasts
 *  - handle CLIENT_DISCONNECT and PLAYER_LEAVE broadcasts
 *  - route input messages into session_entity_map_ InputBuffer
 */
class Multiplayer {
    public:
        Multiplayer(ServerECS &ecs);
        ~Multiplayer();

        // Allow GameServer to set the server pointer so Multiplayer can trigger broadcasts
        void set_udp_server(UdpServer* server);

        // Handle a raw received packet (first byte is message_type; payload after)
        void handle_packet(const std::string &session_id, const std::vector<char> &data);

        // Spawn all connected players when game starts
        void spawn_all_players();
        
        // Broadcast enemy spawn to all clients
        void broadcast_enemy_spawn(entity ent, uint8_t enemy_type, float x, float y);

    private:
        ServerECS &ecs_;
        UdpServer* udp_server_{nullptr};

        // Internal helpers, extracted from previous monolithic implementation
        void handle_client_connect(const std::string &session_id, const std::vector<char> &payload);
        void handle_client_disconnect(const std::string &session_id, const std::vector<char> &payload);
        void handle_client_ready(const std::string &session_id, const std::vector<char> &payload);
        void handle_client_unready(const std::string &session_id, const std::vector<char> &payload);
        void handle_game_message(const std::string &session_id, uint8_t msg_type, const std::vector<char> &payload);
        // Smaller helper functions to simplify connect handling
        std::pair<float,float> choose_spawn_position();
        entity spawn_player_entity(float x, float y);
        void send_server_accept(const std::string &session_id, uint32_t token, float x, float y);
        void broadcast_new_player_spawn(const std::string &session_id, uint32_t token, entity player_ent, float x, float y);
    };

} // namespace RType::Network
