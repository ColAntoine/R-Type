/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ParabolSys system implementation
*/

#include <memory>

#include "ParabolSys.hpp"

void ParabolSys::update(registry& r, float dt)
{
    handleParabols(r);
}

#include <iostream>
void ParabolSys::handleParabols(registry &r)
{
    auto pos_arr = r.get_if<position>();
    auto gravity_arr = r.get_if<Gravity>();
    auto parabol_arr = r.get_if<Parabol>();
    auto vel_arr = r.get_if<velocity>();

    if (!pos_arr || !gravity_arr || !parabol_arr || !vel_arr) return;

    for (auto [pos, gravity, parabol, vel, ent] : zipper(*pos_arr, *gravity_arr, *parabol_arr, *vel_arr)) {
        float y_distance = pos.y - parabol._startY;

        if (std::abs(y_distance) >= parabol._range && y_distance > 0 && !parabol._flickedUp) {
            parabol._flickedUp = true;
            gravity._force = -gravity._force;
        }
        if (std::abs(y_distance) >= parabol._range && y_distance < 0 && parabol._flickedUp) {
            parabol._flickedUp = false;
            gravity._force = -gravity._force;
        }
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<ParabolSys>();
    }
}
