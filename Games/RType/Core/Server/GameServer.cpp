/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameServer Implementation - Replaces enemy_manager
*/

#include "GameServer.hpp"
#include "ECS/Components.hpp"
#include "Entity/Components/Enemy/Enemy.hpp"
#include <iostream>
#include <thread>

// ============================================================================
// Initialization
// ============================================================================

bool GameServer::initialize(uint16_t port) {
    port_ = port;
    std::cout << "┌────────────────────────────────────────┐" << std::endl;
    std::cout << "│   R-Type Server Initialization         │" << std::endl;
    std::cout << "└────────────────────────────────────────┘" << std::endl;
    std::cout << "Port: " << port_ << std::endl;

    try {
        // ① Setup ECS base (load components)
        std::cout << "⚙️  Setting up ECS base..." << std::endl;
        setup_ecs_base();

        // ② Load shared systems (physics, collision, AI)
        std::cout << "⚙️  Loading shared systems..." << std::endl;
        load_shared_systems();

        // ③ Load server-specific systems (spawn, network sync, cleanup)
        std::cout << "⚙️  Loading server systems..." << std::endl;
        load_specific_systems();

        // ④ Initialize network (UDP server)
        std::cout << "⚙️  Setting up network..." << std::endl;
        if (!setup_network()) {
            return false;
        }

        initialized_ = true;
        last_update_ = std::chrono::steady_clock::now();
        std::cout << "✅ Server initialized successfully" << std::endl;
        std::cout << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "❌ Initialization failed: " << e.what() << std::endl;
        return false;
    }
}

// ============================================================================
// Load Systems (override from GameCore)
// ============================================================================

void GameServer::load_specific_systems() {
    // Load server-only systems (EnemySpawn, NetworkSync, Cleanup, AI)
    system_loader_.load_system_from_so("lib/systems/libgame_EnemySpawnSystem.so");
    system_loader_.load_system_from_so("lib/systems/libgame_NetworkSyncSystem.so");
    system_loader_.load_system_from_so("lib/systems/libgame_EnemyCleanup.so");
    system_loader_.load_system_from_so("lib/systems/libgame_EnemyAI.so");

    std::cout << "✅ Server systems loaded (" 
              << system_loader_.get_system_count() << " systems)" << std::endl;
}

// ============================================================================
// Network Setup
// ============================================================================

bool GameServer::setup_network() {
    try {
        // Create ASIO io_context
        io_context_ = std::make_shared<asio::io_context>();
        
        // Create UDP server (dereference shared_ptr to get io_context&)
        server_ = std::make_shared<RType::Network::UdpServer>(*io_context_, port_);
        
        std::cout << "✅ Network layer initialized on port " << port_ << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Network initialization failed: " << e.what() << std::endl;
        return false;
    }
}

// ============================================================================
// Main Loop
// ============================================================================

void GameServer::run() {
    if (!initialized_) {
        std::cerr << "❌ Server not initialized" << std::endl;
        return;
    }

    running_ = true;
    std::cout << "┌────────────────────────────────────────┐" << std::endl;
    std::cout << "│   🚀 GameServer Running                │" << std::endl;
    std::cout << "└────────────────────────────────────────┘" << std::endl;
    std::cout << "Port: " << port_ << std::endl;
    std::cout << "Tick rate: " << (1.0f / fixed_timestep_) << " TPS" << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl;
    std::cout << std::endl;

    // Start network I/O in separate thread
    std::thread network_thread([this]() {
        try {
            std::cout << "[NETWORK] Thread started" << std::endl;
            server_->start();
        } catch (const std::exception& e) {
            std::cerr << "[NETWORK] Error: " << e.what() << std::endl;
            running_ = false;
        }
    });

    // Main game loop (fixed timestep at 60 TPS)
    std::cout << "[GAME LOOP] Starting" << std::endl;

    while (running_) {
        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - last_update_).count();

        if (dt >= fixed_timestep_) {
            update(fixed_timestep_);
            last_update_ = now;
        } else {
            // Sleep to avoid busy-waiting (1ms granularity)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    // Cleanup
    std::cout << std::endl;
    std::cout << "Stopping server..." << std::endl;
    server_->stop();
    if (network_thread.joinable()) {
        network_thread.join();
    }
    std::cout << "✅ GameServer stopped cleanly" << std::endl;
}

// ============================================================================
// Update (called at fixed timestep)
// ============================================================================

void GameServer::update(float dt) {
    // Update all ECS systems in order (via DLLoader)
    // The systems are executed in the order they were loaded:
    // 1. EnemySpawnSystem    → Spawn enemies if timer elapsed
    // 2. NetworkSyncSystem   → Process client inputs
    // 3. EnemyCleanupSystem  → Destroy enemies out of bounds
    // 4. EnemyAISystem       → Move enemies (zigzag, sine wave, etc.)
    // 5. Position system     → Apply velocity to positions (shared)
    // 6. Collision system    → Detect collisions (shared)
    // 7. LifeTime system     → Destroy expired entities (shared)
    system_loader_.update_all_systems(ecs_registry_, dt);

    // Debug: Print stats every 5 seconds
    static float debug_timer = 0.0f;
    debug_timer += dt;
    if (debug_timer >= 5.0f) {
        // Count entities by component type
        auto* enemies = ecs_registry_.get_if<enemy>();
        auto* positions = ecs_registry_.get_if<position>();
        auto* velocities = ecs_registry_.get_if<velocity>();

        size_t enemy_count = enemies ? enemies->size() : 0;
        size_t position_count = positions ? positions->size() : 0;
        size_t velocity_count = velocities ? velocities->size() : 0;

        std::cout << "[SERVER] "
                  << "Enemies: " << enemy_count
                  << " | Positions: " << position_count
                  << " | Velocities: " << velocity_count
                  << " | Clients: " << (server_ ? server_->get_client_count() : 0)
                  << std::endl;

        debug_timer = 0.0f;
    }
}

// ============================================================================
// Shutdown
// ============================================================================

void GameServer::shutdown() {
    std::cout << "Shutdown signal received" << std::endl;
    running_ = false;
}