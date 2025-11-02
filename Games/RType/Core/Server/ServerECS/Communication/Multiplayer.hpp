#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>

#include "Protocol/Protocol.hpp"
#include "ECS/Entity.hpp"
#include "Network/UDPServer.hpp"

namespace RType::Network {

class ServerECS; // forward
class Lobby;
class InGame;

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
        Multiplayer(ServerECS &ecs, int maxLobbies = 0, int maxPlayers = 2);
        ~Multiplayer();

        // Allow GameServer to set the server pointer so Multiplayer can trigger broadcasts
        void set_udp_server(UdpServer* server);

        // Handle a raw received packet (first byte is message_type; payload after)
        void handle_packet(const std::string &session_id, const std::vector<char> &data);

        // Spawn all connected players when game starts (forwarded to InGame)
        void spawn_all_players();
        // Broadcast position updates for all player entities (called after position system runs)
        void broadcast_loop();
        // Broadcast enemy spawn to all clients
        void broadcast_enemy_spawn(entity ent, uint8_t enemy_type, float x, float y);
        // Broadcast entity destruction to all clients
        void broadcast_entity_destroy(entity ent, uint8_t reason = 0);

    private:
        ServerECS &ecs_;
        UdpServer* udp_server_{nullptr};
        int max_lobbies_{0};
        int max_players_{2};

    // Split lobby / in-game handlers (forward-declared above in namespace)
    std::unique_ptr<Lobby> lobby_;
    std::unique_ptr<InGame> ingame_;
};

} // namespace RType::Network
