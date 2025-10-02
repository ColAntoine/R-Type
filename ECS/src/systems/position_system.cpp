/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Position System Implementation
*/

#include "ecs/systems/position_system.hpp"
#include "ecs/registry.hpp"
#include "ecs/components.hpp"
#include "ecs/zipper.hpp"

void PositionSystem::update(registry& r, float dt) {
    auto *pos_arr = r.get_if<position>();
    auto *vel_arr = r.get_if<velocity>();
    if (!pos_arr || !vel_arr) return;

    for (auto [pos, vel, entity] : zipper(*pos_arr, *vel_arr)) {
        pos.x += vel.vx * dt;
        pos.y += vel.vy * dt;
    }
}

extern "C"
std::unique_ptr<ISystem> create_system() {
    return std::make_unique<PositionSystem>();
}
