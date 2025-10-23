
#include "AGameCore.hpp"
#include <iostream>
#include <chrono>
#include <thread>

AGameCore::AGameCore() {}

AGameCore::~AGameCore() {}

bool AGameCore::init()
{
    std::cout << "AGameCore::init" << std::endl;
    return onInit();
}

void AGameCore::run()
{
    std::cout << "AGameCore::run" << std::endl;
    onRun();

    // Simple loop example: call update at ~60hz until shutdown is requested.
    bool running = true;
    const float dt = 1.0f / 60.0f;
    while (running) {
        auto start = std::chrono::steady_clock::now();
        onUpdate(dt);
        // For a placeholder, break after one iteration to avoid infinite loop
        running = false;
        auto end = std::chrono::steady_clock::now();
        std::this_thread::sleep_for(std::chrono::duration<float>(dt) - (end - start));
    }
}

void AGameCore::update(float delta)
{
    onUpdate(delta);
}

void AGameCore::shutdown()
{
    std::cout << "AGameCore::shutdown" << std::endl;
    onShutdown();
}
