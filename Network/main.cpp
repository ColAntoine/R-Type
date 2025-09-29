#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include "server.hpp"

UDPServer* server_instance = nullptr;

void signal_handler(int signal) {
    std::cout << "\nReceived signal " << signal << ". Shutting down server..." << std::endl;
    if (server_instance) {
        server_instance->stop();
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

void instructions(int port) {
    std::cout << "Server is running. Press Ctrl+C to stop." << std::endl;
    std::cout << "Clients can connect to UDP port " << port << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  'info' - Show server information" << std::endl;
    std::cout << "  'broadcast <message>' - Send message to all clients" << std::endl;
    std::cout << "  'quit' - Stop the server" << std::endl;
    std::cout << std::endl;
}

void commands_loop(UDPServer &server) {
    std::string input;
    while (server.is_running() && std::getline(std::cin, input)) {
        if (input == "quit") {
            std::cout << "Stopping server..." << std::endl;
            break;
        } else if (input == "info") {
            server.print_server_info();
        } else if (input.substr(0, 9) == "broadcast" && input.length() > 10) {
            std::string message = input.substr(10);
            server.broadcast_message(message);
        } else if (!input.empty()) {
            std::cout << "Unknown command. Available: info, broadcast <message>, quit" << std::endl;
        }
    }
}

int main(int ac, char** av) {
    std::cout << "=== R-Type UDP Server ===" << std::endl;

    int port = get_port(ac, av);

    // Set up signal handling for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Create and initialize server
    UDPServer server;
    server_instance = &server;
    if (!server.initialize(port)) {
        std::cerr << "Failed to initialize server on port " << port << std::endl;
        return 1;
    }
    if (!server.start()) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }
    instructions(port);
    commands_loop(server);
    server.stop();
    std::cout << "Server shut down successfully." << std::endl;
    return 0;
}
