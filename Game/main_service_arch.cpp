#include "application.hpp"
#include <iostream>
#include <csignal>

// Global application instance for signal handling
Application* app_instance = nullptr;

void signal_handler(int signal) {
    std::cout << "\nReceived signal " << signal << ". Shutting down..." << std::endl;
    if (app_instance) {
        app_instance->shutdown();
    }
}

int start_app(const std::string& server_ip, int server_port)
{
    Application app;
    app_instance = &app;

    try {
        if (!app.initialize(server_ip, server_port)) {
            std::cerr << "Failed to initialize application" << std::endl;
            return 1;
        }

        app.run();
        app.shutdown();

    } catch (const std::exception& e) {
        std::cerr << "Application error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Application terminated gracefully" << std::endl;
    return 0;
}

int main(int argc, char* argv[]) {
    std::cout << "=== R-Type Service Architecture Client ===" << std::endl;

    std::string server_ip = "127.0.0.1";
    int server_port = 8080;
    if (argc >= 2) {
        server_ip = argv[1];
    }
    if (argc >= 3) {
        server_port = std::atoi(argv[2]);
    }

    std::cout << "Connecting to server: " << server_ip << ":" << server_port << std::endl;

    // Set up signal handling
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // Create and run application
    return start_app(server_ip, server_port);
}