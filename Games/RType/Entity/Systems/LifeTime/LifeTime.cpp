/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Lifetime System Implementation
*/

#include <vector>
#include <algorithm>
#include <iostream>

#include "Entity/Systems/LifeTime/LifeTime.hpp"
#include "Entity/Components/LifeTime/LifeTime.hpp"
#include "Entity/Components/Spawner/Spawner.hpp"
#include "Entity/Components/Enemy/Enemy.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Renderer/RenderManager.hpp"

void LifetimeSystem::update(registry& r, float dt) {
    auto *lifetime_arr = r.get_if<lifetime>();
    auto *pos_arr = r.get_if<position>();
    auto *spawner_arr = r.get_if<spawner>();

    if (!lifetime_arr || !pos_arr) return;

    auto& renderManager = RenderManager::instance();
    int window_width = renderManager.get_screen_infos().getWidth();

    std::vector<entity> entities_to_remove;

    for (auto [life_comp, pos_comp, ent] : zipper(*lifetime_arr, *pos_arr)) {
        life_comp.current_time += dt;

        bool should_destroy = false;

        // Check if lifetime expired
        if (life_comp.current_time >= life_comp.max_lifetime) {
            should_destroy = true;
        }

        // Check if entity is offscreen and should be destroyed
        if (life_comp.destroy_offscreen) {
            if (pos_comp.x < -100 || pos_comp.x > window_width + 100) {
                should_destroy = true;
            }
        }

        if (should_destroy) {
            entities_to_remove.push_back(entity(ent));
        }
    }

    // Remove expired entities and update spawner counts
    for (entity ent : entities_to_remove) {
        // Check if entity still exists before removing
        auto *health_check = r.get_if<lifetime>();
        if (!health_check || !health_check->has(static_cast<size_t>(ent))) {
            continue; // Entity already removed, skip
        }

        // If entity was an enemy, decrease spawner count
        if (r.get_if<Enemy>() && r.get_if<Enemy>()->has(static_cast<size_t>(ent))) {
            if (spawner_arr) {
                for (auto [spawn_comp, spawn_entity] : zipper(*spawner_arr)) {
                    spawn_comp.current_count = std::max(0, spawn_comp.current_count - 1);
                }
            }
        }

        r.kill_entity(ent);
    }
}

DLL_EXPORT ISystem* create_system() {
    try {
        return new LifetimeSystem();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}
