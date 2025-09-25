#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include "client.hpp"

UDPClient* client_instance = nullptr;

void signal_handler(int signal) {
    std::cout << "\nReceived signal " << signal << ". Disconnecting client..." << std::endl;
    if (client_instance) {
        client_instance->disconnect();
    }
}

void print_help() {
    std::cout << "=== R-Type UDP Client ===" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  'info' - Show client information" << std::endl;
    std::cout << "  'msg <message>' - Send message to server" << std::endl;
    std::cout << "  'help' - Show this help" << std::endl;
    std::cout << "  'quit' - Disconnect and exit" << std::endl;
    std::cout << std::endl;
}

void commands_loop(UDPClient& client) {
    std::string input;
    while (client.is_connected() && std::getline(std::cin, input)) {
        if (input == "quit") {
            std::cout << "Disconnecting..." << std::endl;
            break;
        } else if (input == "info") {
            client.print_client_info();
        } else if (input == "help") {
            print_help();
        } else if (input.substr(0, 4) == "msg " && input.length() > 4) {
            std::string message = input.substr(4);
            client.send_message(message);
            std::cout << "Sent: " << message << std::endl;
        } else if (!input.empty()) {
            // Send any other input as a message
            client.send_message(input);
            std::cout << "Sent: " << input << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    std::cout << "=== R-Type UDP Client ===" << std::endl;

    // Default server connection
    std::string server_ip = "127.0.0.1";
    int server_port = 8080;

    // Check command line arguments for custom server
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

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    UDPClient client;
    client_instance = &client;

    if (!client.connect_to_server(server_ip, server_port)) {
        std::cerr << "Failed to connect to server " << server_ip << ":" << server_port << std::endl;
        return 1;
    }
    client.start_receiving();
    print_help();
    std::cout << "Connected to server. Type messages or commands:" << std::endl;
    commands_loop(client);
    client.disconnect();
    std::cout << "Client shut down successfully." << std::endl;
    return 0;
}
