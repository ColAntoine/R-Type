/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Spawn System Implementation (Disabled for Multiplayer)
*/

#include "ecs/systems/spawn_system.hpp"
#include "ecs/registry.hpp"
#include "ecs/components.hpp"

void SpawnSystem::update(registry& r, float dt) {
    // This function is disabled for multiplayer synchronization
    // Enemies are now managed by the server and sent to clients via ENTITY_UPDATE messages
    return;
}

std::unique_ptr<ISystem> create_system() {
    return std::make_unique<SpawnSystem>();
}
