/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameServer - Dedicated game server (headless, no rendering)
*/

#pragma once

#include "Core/GameCore.hpp"
#include "udp_server.hpp"
#include "protocol.hpp"
#include <memory>
#include <chrono>
#include <atomic>

/**
 * @brief Dedicated game server (headless, no rendering)
 *
 * Responsibilities:
 * - Spawn and manage enemies
 * - Run AI logic
 * - Broadcast game state to clients
 * - Handle player connections and inputs
 */
class GameServer : public GameCore {
private:
    // Network
    std::shared_ptr<RType::Network::UdpServer> server_;
    std::shared_ptr<asio::io_context> io_context_;

    // Server state
    std::atomic<bool> running_{false};
    std::chrono::steady_clock::time_point last_update_;

    // Configuration
    uint16_t port_{8080};
    float fixed_timestep_{1.0f / 60.0f};  // 60 TPS
    float world_width_{1920.0f};
    float world_height_{1080.0f};

public:
    GameServer() = default;
    ~GameServer() override = default;

    /**
     * @brief Initialize the game server
     * @param port Server listening port
     * @return true if successful
     */
    bool initialize(uint16_t port);

    /**
     * @brief Start the server loop (blocking)
     */
    void run();

    /**
     * @brief Stop the server gracefully
     */
    void shutdown();

    /**
     * @brief Get the UDP server instance
     */
    std::shared_ptr<RType::Network::UdpServer> get_server() const { return server_; }

protected:
    /**
     * @brief Load server-specific systems (override from GameCore)
     */
    void load_specific_systems() override;

private:
    /**
     * @brief Initialize network layer
     */
    bool setup_network();

    /**
     * @brief Update game logic (called at fixed timestep)
     */
    void update(float dt);
};