#include "Application.hpp"
#include "Core/PlatformMacros.hpp"
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

int start_app()
{
    Application app;
    app_instance = &app;

    try {
        if (!app.initialize()) {
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

int main(MAYBE_UNUSED int argc, MAYBE_UNUSED char* argv[]) {
    std::cout << "=== R-Type Service Architecture Client ===" << std::endl;
    std::cout << "Starting in offline mode - use UI to connect to server" << std::endl;

    // Set up signal handling
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // Create and run application (no auto-connection)
    return start_app();
}