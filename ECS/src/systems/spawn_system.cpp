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

extern "C" ISystem* create_system() {
    return new SpawnSystem();
}

extern "C" void destroy_system(ISystem* system) {
    delete system;
}
