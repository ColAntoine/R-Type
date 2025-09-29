#include <iostream>
#include <csignal>
#include "client.hpp"
#include "game_window.hpp"
#include "game_engine.hpp"

// Global instances for signal handling
UDPClient* client_instance = nullptr;
GameWindow* window_instance = nullptr;
GameEngine* engine_instance = nullptr;

void signal_handler(int signal) {
    std::cout << "\nReceived signal " << signal << ". Shutting down..." << std::endl;
    if (engine_instance) {
        engine_instance->shutdown();
    }
    if (client_instance) {
        client_instance->disconnect();
    }
    if (window_instance) {
        window_instance->close();
    }
}

int main(int argc, char* argv[]) {
    std::cout << "=== R-Type UDP Client ===" << std::endl;

    // Parse command line arguments
    std::string server_ip = "127.0.0.1";
    int server_port = 8080;

    if (argc > 1) {
        server_ip = argv[1];
    }
    if (argc > 2) {
        server_port = std::atoi(argv[2]);
        if (server_port <= 0 || server_port > 65535) {
            std::cerr << "Invalid port number. Using default port 8080." << std::endl;
            server_port = 8080;
        }
    }

    // Set up signal handling
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Create core components
    UDPClient client;
    GameWindow window(1000, 860, "R-Type Client");
    GameEngine engine;

    // Set global instances for signal handling
    client_instance = &client;
    window_instance = &window;
    engine_instance = &engine;

    // Connect to server
    std::cout << "Connecting to server " << server_ip << ":" << server_port << "..." << std::endl;
    if (!client.connect_to_server(server_ip, server_port)) {
        std::cerr << "Failed to connect to server " << server_ip << ":" << server_port << std::endl;
        return 1;
    }
    std::cout << "Successfully connected to server!" << std::endl;

    // Initialize graphics window
    if (!window.initialize()) {
        std::cerr << "Failed to initialize game window" << std::endl;
        client.disconnect();
        return 1;
    }

    // Initialize game engine BEFORE starting networking to set up callback
    if (!engine.initialize(&window, &client)) {
        std::cerr << "Failed to initialize game engine" << std::endl;
        client.disconnect();
        window.close();
        return 1;
    }

    // Start networking AFTER callback is set up
    client.start_receiving();

    // Run the game
    engine.run();

    // Clean shutdown
    engine.shutdown();
    client.disconnect();
    window.close();

    std::cout << "Client shut down successfully." << std::endl;
    return 0;
}