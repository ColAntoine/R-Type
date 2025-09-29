#include "network_manager.hpp"
#include "udp_server.hpp"
#include "message_handler.hpp"
#include "game_handlers.hpp"
#include "protocol.hpp"
#include <iostream>
#include <csignal>
#include <string>

using namespace RType::Network;

std::shared_ptr<NetworkManager> g_network_manager = nullptr;

void signal_handler(int signal) {
    std::cout << "\nReceived signal " << signal << ". Shutting down server..." << std::endl;
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
        else if (!input.empty()) {
            std::cout << "Unknown command. Available: info, clients, quit" << std::endl;
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
        size_t thread_count = std::max(2u, std::thread::hardware_concurrency());
        if (!g_network_manager->start(thread_count)) {
            std::cerr << "Failed to start network manager" << std::endl;
            return 1;
        }
        print_instructions(port);
        commands_loop(g_network_manager);
        g_network_manager->stop();
        std::cout << "Server shut down successfully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}