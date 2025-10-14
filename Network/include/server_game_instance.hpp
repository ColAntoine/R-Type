#pragma once

#include "../../ECS/include/ECS/Registry.hpp"
#include "../../ECS/include/ECS/DLLoader.hpp"
#include "../../ECS/include/ECS/Components.hpp"
#include "udp_server.hpp"
#include <memory>
#include <thread>
#include <atomic>
#include <chrono>

namespace RType::Network {

    /**
     * @brief Server-side game instance that runs the game simulation without display
     * 
     * This class manages the server-side ECS systems and game loop.
     * It runs all the game logic including enemy AI, spawning, physics, etc.
     * but excludes rendering and client-specific systems.
     */
    class ServerGameInstance {
    public:
        ServerGameInstance();
        ~ServerGameInstance();

        /**
         * @brief Initialize the game instance with server reference
         * @param server Shared pointer to the UDP server
         * @return true if initialization succeeded
         */
        bool initialize(std::shared_ptr<UdpServer> server);

        /**
         * @brief Start the game simulation
         * @param tickrate Game simulation tickrate (ticks per second)
         * @return true if startup succeeded
         */
        bool start(int tickrate = 60);

        /**
         * @brief Stop the game simulation
         */
        void stop();

        /**
         * @brief Check if the game is currently running
         */
        bool is_running() const { return running_; }

        /**
         * @brief Get the ECS registry
         */
        registry& get_registry() { return ecs_registry_; }

        /**
         * @brief Update the game simulation (called by game loop)
         * @param delta_time Time elapsed since last update
         */
        void update(float delta_time);

    private:
        void setup_ecs();
        void load_server_systems();
        void game_loop();

        // ECS
        registry ecs_registry_;
        DLLoader system_loader_;
        IComponentFactory* component_factory_;

        // Game state
        std::atomic<bool> running_;
        std::atomic<bool> initialized_;
        
        // Server reference
        std::shared_ptr<UdpServer> server_;

        // Game loop
        std::thread game_thread_;
        int tickrate_;
        std::chrono::milliseconds tick_duration_;
    };

} // namespace RType::Network