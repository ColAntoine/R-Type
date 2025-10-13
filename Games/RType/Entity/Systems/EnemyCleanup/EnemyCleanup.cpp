/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Enemy Cleanup System Implementation
*/

#include "EnemyCleanup.hpp"
#include "Entity/Systems/NetworkSyncSystem/NetworkSyncSystem.hpp"
#include <iostream>

EnemyCleanupSystem::EnemyCleanupSystem(NetworkSyncSystem* net_sync)
    : network_sync_system_(net_sync)
{
}

void EnemyCleanupSystem::update(registry& r, float dt) {
    auto* enemies = r.get_if<Enemy>();
    auto* positions = r.get_if<position>();
    auto* sync_comps = r.get_if<network_sync>();

    if (!enemies || !positions || !sync_comps) return;

    std::vector<entity> to_destroy;
    // Check for out-of-bounds enemies
    for (auto [enm, pos, sync, index] : zipper(*enemies, *positions, *sync_comps)) {
        bool should_destroy = false;
        // Out of bounds (left)
        if (pos.x < world_bounds_left_) {
            should_destroy = true;
            std::cout << "Enemy " << sync.network_id << " out of bounds (x=" 
                     << pos.x << ")" << std::endl;
        }

        // TODO: when health component is implemented, check health here
        if (should_destroy) {
            to_destroy.push_back(entity(index));
        }
    }

    // Destroy entities
    for (entity e : to_destroy) {
        // Broadcast destruction
        if (sync_comps && e < sync_comps->size()) {
            uint32_t net_id = (*sync_comps)[e].network_id;
            if (network_sync_system_) {
                network_sync_system_->broadcast_destroy(net_id);
            }
        }
        // Kill entity in registry
        r.kill_entity(e);
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<EnemyCleanupSystem>();
    }
}