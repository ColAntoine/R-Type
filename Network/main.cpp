#include "network_manager.hpp"
#include "udp_server.hpp"
#include "message_handler.hpp"
#include "game_handlers.hpp"
#include "protocol.hpp"
#if defined(_WIN32)
#  include "ECS/WindowsLoader.hpp"
    using ServerDLLoader = WindowsLoader;
#elif defined(__APPLE__)
#  include "ECS/MacOs.hpp"
    using ServerDLLoader = MacOs;
#else
#  include "ECS/LinuxLoader.hpp"
    using ServerDLLoader = LinuxLoader;
#endif
#include "ECS/Registry.hpp"
#include "enemy_manager.hpp"
#include <iostream>
#include <csignal>
#include <string>
#include <chrono>
#include <thread>

using namespace RType::Network;

std::shared_ptr<NetworkManager> g_network_manager = nullptr;
std::unique_ptr<RType::Network::ServerEnemyManager> g_enemy_manager = nullptr;

// Server-side ECS (for authoritative enemy spawning)
static registry g_server_ecs_registry;
static ServerDLLoader g_server_dl_loader;

void game_loop() {
    const float target_fps = 60.0f;
    const auto frame_duration = std::chrono::milliseconds(static_cast<long>(1000.0f / target_fps));

    auto last_time = std::chrono::high_resolution_clock::now();

    std::cout << "Game loop started (60 FPS)" << std::endl;

    while (g_network_manager && g_network_manager->is_running()) {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto delta_time = std::chrono::duration<float>(current_time - last_time).count();
        last_time = current_time;

        // Update server-side enemy manager (spawns and broadcasts entity create messages)
        if (g_enemy_manager) {
            g_enemy_manager->update(delta_time);
        }

        // Sleep to maintain target FPS
        std::this_thread::sleep_for(frame_duration);
    }

    std::cout << "Game loop stopped" << std::endl;
}

void signal_handler(int signal) {
    std::cout << "\nReceived signal " << signal << ". Shutting down server..." << std::endl;
    // Enemies are managed by ECS systems; no explicit enemy manager cleanup here.
    if (g_network_manager) {
        g_network_manager->stop();
    }
}

int get_port(int ac, char** av) {
    int port = 8080; // default
    if (ac > 1) {
        port = std::atoi(av[1]);
        if (port <= 0 || port > 65535) {
            std::cerr << "Invalid port number. Using default port 8080." << std::endl;
            port = 8080;
        }
    }
    return port;
}

void print_instructions(int port) {
    std::cout << "=== R-Type Server (New Architecture) ===" << std::endl;
    std::cout << "Server is running on port " << port << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  ✓ Asio-based asynchronous networking" << std::endl;
    std::cout << "  ✓ Modular message handling system" << std::endl;
    std::cout << "  ✓ Session-based client management" << std::endl;
    std::cout << "  ✓ Clean protocol implementation" << std::endl;
    std::cout << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  'info'    - Show server information" << std::endl;
    std::cout << "  'clients' - List connected clients" << std::endl;
    std::cout << "  'quit'    - Stop the server" << std::endl;
    std::cout << "Press Ctrl+C to stop gracefully." << std::endl;
    std::cout << std::endl;
}

void setup_message_handlers(std::shared_ptr<UdpServer> server) {
    // Create message dispatcher
    auto dispatcher = std::make_shared<MessageDispatcher>();

    // Register message handlers
    dispatcher->register_handler(
        static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_CONNECT),
        std::make_shared<ConnectionHandler>(server)
    );

    dispatcher->register_handler(
        static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_READY),
        std::make_shared<ClientReadyHandler>(server)
    );

    dispatcher->register_handler(
        static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_DISCONNECT),
        std::make_shared<ClientDisconnectHandler>(server)
    );

    dispatcher->register_handler(
        static_cast<uint8_t>(RType::Protocol::GameMessage::POSITION_UPDATE),
        std::make_shared<PositionUpdateHandler>(server)
    );

    dispatcher->register_handler(
        static_cast<uint8_t>(RType::Protocol::SystemMessage::PING),
        std::make_shared<PingHandler>()
    );

    dispatcher->register_handler(
        static_cast<uint8_t>(RType::Protocol::GameMessage::PLAYER_SHOOT),
        std::make_shared<PlayerShootHandler>(server)
    );    // Set the message handler for the server
    server->set_message_handler([dispatcher](std::shared_ptr<Session> session, const char* data, size_t size) {
        dispatcher->dispatch_message(session, data, size);
    });
}

void commands_loop(std::shared_ptr<NetworkManager> network_manager) {
    std::string input;
    auto server = network_manager->get_server();

    while (network_manager->is_running() && std::getline(std::cin, input)) {
        if (input == "quit") {
            std::cout << "Stopping server..." << std::endl;
            break;
        }
        else if (input == "info") {
            std::cout << "=== Server Information ===" << std::endl;
            std::cout << "Status: " << (network_manager->is_running() ? "Running" : "Stopped") << std::endl;
            std::cout << "Connected clients: " << server->get_client_count() << std::endl;
            std::cout << "Architecture: Asio-based modular design" << std::endl;
            std::cout << std::endl;
        }
        else if (input == "clients") {
            std::cout << "=== Connected Clients ===" << std::endl;
            std::cout << "Total clients: " << server->get_client_count() << std::endl;
            std::cout << std::endl;
        }
        else if (input == "enemies") {
            std::cout << "Enemy info is managed by ECS systems (EnemyAI / EnemySpawnSystem / EnemyCleanup)." << std::endl;
            std::cout << "Use the ECS tools or connect a game instance to inspect enemies." << std::endl;
        }
        else if (input.substr(0, 6) == "spawn ") {
            std::cout << "Manual spawn via server CLI is deprecated. Use ECS spawn system or send a game message to request a spawn." << std::endl;
        }
        else if (input == "clear") {
            std::cout << "Clearing enemies is handled by ECS systems. No action taken from server CLI." << std::endl;
        }
        else if (input == "ready") {
            auto server = g_network_manager->get_server();
            if (server) {
                std::cout << "=== Client Readiness Status ===" << std::endl;
                std::cout << "Total clients: " << server->get_client_count() << std::endl;
                std::cout << "Ready clients: " << server->get_ready_client_count() << std::endl;
                std::cout << "All clients ready: " << (server->are_all_clients_ready() ? "Yes" : "No") << std::endl;
                std::cout << "Game logic running: " << (server->should_run_game_logic() ? "Yes" : "No") << std::endl;
                std::cout << std::endl;
            } else {
                std::cout << "Server not initialized" << std::endl;
            }
        }
        else if (!input.empty()) {
            std::cout << "Unknown command. Available: info, clients, enemies, spawn <type>, clear, ready, quit" << std::endl;
        }
    }
}

int main(int ac, char** av) {
    int port = get_port(ac, av);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    try {
        g_network_manager = std::make_shared<NetworkManager>();
        if (!g_network_manager->initialize(port)) {
            std::cerr << "Failed to initialize network manager on port " << port << std::endl;
            return 1;
        }
        setup_message_handlers(g_network_manager->get_server());

    // Load ECS component definitions into the server registry so server can spawn entities
    if (!g_server_dl_loader.load_components("lib/libECS.so", g_server_ecs_registry)) {
        std::cerr << "Warning: Failed to load ECS components for server; enemy spawning will still broadcast but server entities may not be tracked." << std::endl;
    }
    IComponentFactory* server_factory = g_server_dl_loader.get_factory();

    // Create server-side enemy manager and configure bounds (pass registry + factory)
    g_enemy_manager = std::make_unique<RType::Network::ServerEnemyManager>(g_network_manager->get_server(), &g_server_ecs_registry, server_factory);
    g_enemy_manager->set_world_bounds(1024.0f, 768.0f);

    // Enemy management moved to ECS systems (EnemyAI, EnemySpawnSystem, EnemyCleanup, NetworkSyncSystem)
    // No local EnemyManager is instantiated here anymore.

        size_t thread_count = std::max(2u, std::thread::hardware_concurrency());
        if (!g_network_manager->start(thread_count)) {
            std::cerr << "Failed to start network manager" << std::endl;
            return 1;
        }

        print_instructions(port);

        // Start game loop in a separate thread
        std::thread game_thread(game_loop);

        commands_loop(g_network_manager);

        // Cleanup: ECS-managed systems should handle entity cleanup. Nothing to do here.

        // Wait for game thread to finish
        if (game_thread.joinable()) {
            game_thread.join();
        }

        g_network_manager->stop();
        std::cout << "Server shut down successfully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}