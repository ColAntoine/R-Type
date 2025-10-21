/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GravitySys system implementation
*/

#include <memory>

#include "GravitySys.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components.hpp"
#include "ECS/Zipper.hpp"

#include "Entity/Components/Gravity/Gravity.hpp"

// GravitySys acceleration in pixels (game units) per second squared

void GravitySys::update(registry& r, float dt) {
    applyGravitySys(r, dt);
}

void GravitySys::applyGravitySys(registry &r, float dt)
{
    auto *vel_arr = r.get_if<velocity>();
    auto *pos_arr = r.get_if<position>();
    auto *gravity_arr = r.get_if<Gravity>();

    if (!vel_arr || !pos_arr || !gravity_arr) return;

    for (auto [pos, vel, gravity, entity] : zipper(*pos_arr, *vel_arr, *gravity_arr)) {
        // apply GravitySys to vertical velocity
        vel.vy += gravity._force * dt;
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<GravitySys>();
    }
}
