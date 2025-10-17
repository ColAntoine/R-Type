
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

void AGameCore::RegisterComponents(registry &r) {
    RegisterAllComponents(r);

    // Register game-specific components
    r.register_component<controllable>();
    r.register_component<Enemy>();
    r.register_component<Gravity>();
    r.register_component<Health>();
    r.register_component<lifetime>();
    r.register_component<Projectile>();
    r.register_component<remote_player>();
    r.register_component<Score>();
    r.register_component<spawner>();
    r.register_component<Weapon>();
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
