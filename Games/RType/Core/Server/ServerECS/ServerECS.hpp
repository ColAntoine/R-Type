#pragma once

#include "Protocol/MessageQueue.hpp"
#include "ECS/ILoader.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components/InputBuffer.hpp"
#include <unordered_map>
#include "Core/AGameCore.hpp"
#include <string>
#include <chrono>
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"

namespace RType::Network {

class Multiplayer;

class UdpServer;

class ServerECS {
    public:
        ServerECS(int maxLobbies = 0, int maxPlayers = 2);
        ~ServerECS();

        // Initialize components/systems from shared object (optional)
        bool init(const std::string& components_so = "build/lib/libECS.so");

        // Attach the network message queue used by the UDP server
        void set_message_queue(MessageQueue* q);

        // Process all pending network packets (drain queue and convert to net_input components)
        void process_packets();

        // Run ECS systems for one tick (dt in seconds)
        void tick(float dt);

        void set_send_callback(std::function<void(const std::string&, const std::vector<uint8_t>&)> cb) { send_callback_ = std::move(cb); }
        // Install the server UDP instance into multiplayer so it can trigger broadcasts directly
        void set_udp_server(UdpServer* server);

        // Register a callback invoked when a client requests a new instance (session_id)
        void set_instance_request_callback(std::function<void(const std::string&)> cb) { instance_request_cb_ = std::move(cb); }
        // Register a callback invoked when the server wants to send the current instance list to a specific session
        void set_instance_list_request_callback(std::function<void(const std::string&)> cb) { instance_list_request_cb_ = std::move(cb); }

        IComponentFactory* get_factory() const { return factory_; }
        registry& GetRegistry();
        ILoader& GetILoader() { return *loader_; }

        // Expose multiplayer for game start coordination
        Multiplayer* GetMultiplayer() { return multiplayer_.get(); }

        // Game state flag (set by GameServer when game starts)
        void set_game_started(bool v) { game_started_ = v; }
        bool is_game_started() const { return game_started_; }

    private:
        std::unique_ptr<ILoader> loader_;
        registry registry_;
        IComponentFactory* factory_{nullptr};
        MessageQueue* msgq_{nullptr};
        // Map session id -> player entity id for input routing
        std::unordered_map<std::string, entity> session_entity_map_;
        uint32_t next_session_token_{1};
        std::unordered_map<std::string, uint32_t> session_token_map_;
        // Optional callback to send data back to clients (session_id, raw packet bytes)
        std::function<void(const std::string&, const std::vector<uint8_t>&)> send_callback_;
        // Multiplayer handler
        std::unique_ptr<Multiplayer> multiplayer_;
        // Max lobbies for multi-instance
        int max_lobbies_;
        int max_players_;
        // Callback to request instance creation (front server supplies implementation)
        std::function<void(const std::string&)> instance_request_cb_;
        // Callback to request sending the current instance list to a specific session (front server supplies implementation)
        std::function<void(const std::string&)> instance_list_request_cb_;


        // Allow communication helpers to access internals for now
        friend class Multiplayer;
        friend class Lobby;
        friend class InGame;

    private:
        // Process buffered inputs for all players and apply to components
        void process_inputs();
        // Build and send snapshots to connected clients
        void send_snapshots();

        // Whether the game has started (no new clients should be accepted)
        bool game_started_{false};
};

} // namespace RType::Network
