#include "network_manager.hpp"
#include "udp_server.hpp"
#include "message_handler.hpp"
#include "game_handlers.hpp"
#include "protocol.hpp"
#include "enemy_manager.hpp"
#include <iostream>
#include <csignal>
#include <string>
#include <chrono>
#include <thread>

using namespace RType::Network;

std::shared_ptr<NetworkManager> g_network_manager = nullptr;
std::shared_ptr<EnemyManager> g_enemy_manager = nullptr;

void game_loop() {
    const float target_fps = 60.0f;
    const auto frame_duration = std::chrono::milliseconds(static_cast<long>(1000.0f / target_fps));
    
    auto last_time = std::chrono::high_resolution_clock::now();
    
    std::cout << "Game loop started (60 FPS)" << std::endl;
    
    while (g_network_manager && g_network_manager->is_running()) {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto delta_time = std::chrono::duration<float>(current_time - last_time).count();
        last_time = current_time;
        
        // Update enemy manager
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
    if (g_enemy_manager) {
        g_enemy_manager->clear_all_enemies();
        g_enemy_manager.reset();
    }
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
        std::make_shared<ConnectionHandler>()
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
        std::make_shared<PlayerShootHandler>()
    );    // Set the message handler for the server
    server->set_message_handler([dispatcher](std::shared_ptr<Session> session, const char* data, size_t size) {
        dispatcher->dispatch_message(session, data, size);
    });

    std::cout << "Message handlers registered:" << std::endl;
    std::cout << "  ✓ Connection Handler" << std::endl;
    std::cout << "  ✓ Position Update Handler" << std::endl;
    std::cout << "  ✓ Ping Handler" << std::endl;
    std::cout << "  ✓ Player Shoot Handler" << std::endl;
    std::cout << std::endl;
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
            if (g_enemy_manager) {
                std::cout << "=== Enemy Information ===" << std::endl;
                std::cout << "Active enemies: " << g_enemy_manager->get_enemy_count() << std::endl;
                std::cout << "Max enemies: " << g_enemy_manager->get_max_enemies() << std::endl;
                std::cout << "Spawn interval: " << g_enemy_manager->get_spawn_interval() << "s" << std::endl;
                std::cout << std::endl;
            } else {
                std::cout << "Enemy manager not initialized" << std::endl;
            }
        }
        else if (input.substr(0, 6) == "spawn ") {
            if (g_enemy_manager) {
                try {
                    int enemy_type = std::stoi(input.substr(6));
                    if (enemy_type >= 1 && enemy_type <= 4) {
                        g_enemy_manager->spawn_enemy(enemy_type, 1024.0f, 400.0f);
                        std::cout << "Spawned enemy type " << enemy_type << std::endl;
                    } else {
                        std::cout << "Invalid enemy type. Use 1-4." << std::endl;
                    }
                } catch (const std::exception&) {
                    std::cout << "Usage: spawn <type> (1-4)" << std::endl;
                }
            } else {
                std::cout << "Enemy manager not initialized" << std::endl;
            }
        }
        else if (input == "clear") {
            if (g_enemy_manager) {
                g_enemy_manager->clear_all_enemies();
                std::cout << "Cleared all enemies" << std::endl;
            } else {
                std::cout << "Enemy manager not initialized" << std::endl;
            }
        }
        else if (!input.empty()) {
            std::cout << "Unknown command. Available: info, clients, enemies, spawn <type>, clear, quit" << std::endl;
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
        
        // Initialize enemy manager
        g_enemy_manager = std::make_shared<EnemyManager>(g_network_manager->get_server());
        
        size_t thread_count = std::max(2u, std::thread::hardware_concurrency());
        if (!g_network_manager->start(thread_count)) {
            std::cerr << "Failed to start network manager" << std::endl;
            return 1;
        }
        
        print_instructions(port);
        
        // Start game loop in a separate thread
        std::thread game_thread(game_loop);
        
        commands_loop(g_network_manager);
        
        // Cleanup
        if (g_enemy_manager) {
            g_enemy_manager->clear_all_enemies();
            g_enemy_manager.reset();
        }
        
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