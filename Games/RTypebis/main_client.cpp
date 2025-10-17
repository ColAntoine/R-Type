#include "Core/Client/GameClient.hpp"

#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

static std::atomic_bool g_terminate{false};

void SignalHandler(int signal)
{
    std::cout << "\nReceived signal " << signal << ". Shutting down..." << std::endl;
    g_terminate.store(true);
}

int main(__attribute_maybe_unused__ int argc, __attribute_maybe_unused__ char **av)
{
    IGameCore& client = GameClient::getInstance();

    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);
    try {
        if (!client.init()) {
            std::cerr << "Failed to initialize game client" << std::endl;
            return 84;
        }

        std::cout << "=== Welcome in R-Type ! ===" << std::endl;

        std::thread gameThread([&client]() {
            client.start();
        });

        while (!g_terminate.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        client.shutdown();

        if (gameThread.joinable())
            gameThread.join();

    } catch (const std::exception& e) {
        std::cerr << "Game client error: " << e.what() << std::endl;
        return 84;
    }

    std::cout << "Game client terminated gracefully" << std::endl;
    return 0;
}
