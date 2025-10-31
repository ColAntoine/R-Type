/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Enemy Cleanup System Implementation
*/

#include "EnemyCleanup.hpp"
#include <iostream>

EnemyCleanupSystem::EnemyCleanupSystem(NetworkSyncSystem* net_sync)
    : network_sync_system_(net_sync)
{
}

void EnemyCleanupSystem::update(registry& r, float dt) {
    auto* enemies = r.get_if<Enemy>();
    auto* positions = r.get_if<position>();

    if (!enemies || !positions) return;

    std::vector<entity> to_destroy;
    // Check for out-of-bounds enemies
    for (auto [enm, pos, index] : zipper(*enemies, *positions)) {
        bool should_destroy = false;
        // Out of bounds (left)
        if (pos.x < world_bounds_left_) {
            should_destroy = true;
        }

        // TODO: when health component is implemented, check health here
        if (should_destroy) {
            to_destroy.push_back(entity(index));
        }
    }

    // Destroy entities
    for (entity e : to_destroy) {
        r.kill_entity(e);
    }
}

DLL_EXPORT ISystem* create_system() {
    try {
        return new EnemyCleanupSystem();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}