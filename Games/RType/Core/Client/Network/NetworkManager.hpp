#pragma once

#include <memory>
#include "UDPClient.hpp"
#include "Dispatcher.hpp"
#include "Core/Server/Protocol/Protocol.hpp"
#include "ECS/Registry.hpp"
#include "ECS/DLLoader.hpp"
#include "Handlers/PlayerHandler.hpp"
#include "Handlers/EnemyHandler.hpp"

#include <mutex>
#include <deque>
#include <functional>

class NetworkManager {
    public:
        NetworkManager(std::shared_ptr<UdpClient> client, registry& registry, DLLoader& loader);
        ~NetworkManager();

        // Start/stop receiving
        void start();
        void stop();

        // Register a handler for a specific message type
        void register_handler(uint8_t msg_type, NetworkDispatcher::Handler h);

        // Convenience: register domain handlers (player/enemy)
        void register_default_handlers();
        // Post a job from network thread to be executed on main thread
        void post_to_main(std::function<void()> job);
        // Process pending jobs (call from GameClient::update or run)
        void process_pending();

    private:
        std::shared_ptr<UdpClient> client_;
        NetworkDispatcher dispatcher_;
        registry& registry_;
        DLLoader& loader_;
        PlayerHandler player_handler_;
        EnemyHandler enemy_handler_;
        // pending ENTITY_CREATE messages received before we know our session token
        std::mutex pending_entity_mutex_;
        std::vector<std::vector<char>> pending_entity_creates_;
        // pending jobs to be executed on main thread
        std::mutex pending_mutex_;
        std::deque<std::function<void()>> pending_jobs_;
};
