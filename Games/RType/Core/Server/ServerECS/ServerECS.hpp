#pragma once

#include "Protocol/MessageQueue.hpp"
#include "ECS/DLLoader.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components/NetInput.hpp"
#include "Core/AGameCore.hpp"
#include <string>
#include <chrono>
#include <unordered_map>
#include <vector>

namespace RType::Network {

class UdpServer;

struct PlayerInfo {
    uint32_t player_id;
    std::string name;
    bool is_ready;
    bool is_connected;
};

class ServerECS {
    public:
        ServerECS();
        ~ServerECS();

        // Initialize components/systems from shared object (optional)
        bool init(const std::string& components_so = "lib/libECS.so");

        // Attach the network message queue used by the UDP server
        void set_message_queue(MessageQueue* q);

        // Attach the UDP server for broadcasting messages
        void set_udp_server(UdpServer* server);

        // Process all pending network packets (drain queue and convert to net_input components)
        void process_packets();

        // Run ECS systems for one tick (dt in seconds)
        void tick(float dt);

        // Access registry for advanced ops/tests
        registry& GetRegistry();

        // Lobby management
        void add_player(uint32_t player_id, const std::string& name);
        void remove_player(uint32_t player_id);
        void set_player_ready(uint32_t player_id, bool ready);
        bool are_all_players_ready() const;
        void broadcast_player_list();
        void start_game_if_ready();

    private:
        DLLoader loader_;
        registry registry_;
        IComponentFactory* factory_{nullptr};
        MessageQueue* msgq_{nullptr};
        UdpServer* udp_server_{nullptr};

        // Map player_id to entity
        std::unordered_map<uint32_t, entity> player_entities_;

        // Lobby state
        std::vector<PlayerInfo> players_;
        bool game_started_{false};
};

} // namespace RType::Network
