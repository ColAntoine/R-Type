/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Server entry point - Replaces Network/main.cpp
*/

#include "GameServer.hpp"
#include <iostream>
#include <csignal>
#include <cstdlib>

// Global server instance for signal handling
GameServer* server_instance = nullptr;

void signal_handler(int signal) {
    std::cout << "\n┌────────────────────────────────────────┐" << std::endl;
    std::cout << "│   Signal " << signal << " received                  │" << std::endl;
    std::cout << "└────────────────────────────────────────┘" << std::endl;
    if (server_instance) {
        server_instance->shutdown();
    }
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    uint16_t port = 8080;
    if (argc > 1) {
        port = static_cast<uint16_t>(std::atoi(argv[1]));
        if (port == 0) {
            std::cerr << "❌ Invalid port number: " << argv[1] << std::endl;
            std::cerr << "Usage: " << argv[0] << " [port]" << std::endl;
            return 1;
        }
    }

    std::cout << "┌────────────────────────────────────────┐" << std::endl;
    std::cout << "│          R-Type Server                 │" << std::endl;
    std::cout << "│          Version 1.0                   │" << std::endl;
    std::cout << "└────────────────────────────────────────┘" << std::endl;
    std::cout << std::endl;

    // Setup signal handlers
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    #ifdef SIGQUIT
    std::signal(SIGQUIT, signal_handler);
    #endif

    // Create and initialize server
    GameServer server;
    server_instance = &server;

    if (!server.initialize(port)) {
        std::cerr << "❌ Failed to initialize server" << std::endl;
        return 1;
    }

    // Run server (blocking)
    server.run();

    std::cout << "Server exited" << std::endl;
    return 0;
}