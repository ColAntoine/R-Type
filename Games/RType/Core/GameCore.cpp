/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameCore Implementation
*/

#include "GameCore.hpp"
#include <iostream>
#include <stdexcept>

void GameCore::setup_ecs_base() {
    if (initialized_) return;

    // Load ECS components
    if (!system_loader_.load_components_from_so("build/lib/libECS.so", ecs_registry_)) {
        throw std::runtime_error("Failed to load ECS components");
    }

    component_factory_ = system_loader_.get_factory();
    if (!component_factory_) {
        throw std::runtime_error("Failed to get component factory");
    }

    std::cout << "✅ ECS base initialized" << std::endl;
}

void GameCore::load_shared_systems() {
    // Core physics (shared client/server)
    system_loader_.load_system_from_so("lib/systems/libposition_system.so");
    system_loader_.load_system_from_so("lib/systems/libcollision_system.so");

    // Game logic (shared)
    system_loader_.load_system_from_so("lib/systems/libgame_EnemyAI.so");
    system_loader_.load_system_from_so("lib/systems/libgame_LifeTime.so");

    std::cout << "✅ Shared systems loaded" << std::endl;
}