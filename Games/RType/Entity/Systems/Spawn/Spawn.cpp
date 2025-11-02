/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Spawn System Implementation (Disabled for Multiplayer)
*/

#include "Entity/Systems/Spawn/Spawn.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components.hpp"

void SpawnSystem::update(registry& r, float dt) {
    // Enemies are now managed by the server and sent to clients via ENTITY_UPDATE messages
    return;
}

DLL_EXPORT ISystem* create_system() {
    try {
        return new SpawnSystem();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}