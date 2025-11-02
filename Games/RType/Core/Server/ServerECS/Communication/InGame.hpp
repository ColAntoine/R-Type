#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Protocol/Protocol.hpp"
#include "ECS/Entity.hpp"
#include "Constants.hpp"
#include <cstdint>
// Needed for function signatures that reference sparse_set<position/velocity>
#include "ECS/SparseSet.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"

namespace RType::Network {

class ServerECS;
class UdpServer;

class InGame {
public:
    explicit InGame(ServerECS &ecs);
    ~InGame();

    void set_udp_server(UdpServer* server);

    // Handlers for in-game messages
    void handle_player_input(const std::string &session_id, const std::vector<char> &payload);
    void handle_player_shoot(const std::string &session_id, const std::vector<char> &payload);
    void handle_player_unshoot(const std::string &session_id, const std::vector<char> &payload);
    void handle_game_message(const std::string &session_id, uint8_t msg_type, const std::vector<char> &payload);
    void handle_client_disconnect(const std::string &session_id, const std::vector<char> &payload);

    // Game-phase utilities
    void spawn_all_players();
    void broadcast_loop();
    void broadcast_positions(const std::string& session_id, const entity& player_ent,
                                sparse_set<velocity, std::allocator<velocity>> *vel_arr,
                                sparse_set<position, std::allocator<position>> *pos_arr);
    void broadcast_shoot(const std::string& session_id, const entity& player_ent);
    void broadcast_enemy_spawn(entity ent, uint8_t enemy_type, float x, float y);
    void broadcast_entity_destroy(entity ent, uint8_t reason = 0);

private:
    ServerECS &ecs_;
    UdpServer* udp_server_{nullptr};
    // Tracks whether each connected session is currently holding the shoot input
    // Keyed by session_id (string) -> bool (true while the client is holding fire)
    std::unordered_map<std::string, bool> is_shooting_;
};

} // namespace RType::Network
