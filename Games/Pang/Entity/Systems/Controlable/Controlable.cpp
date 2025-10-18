/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Controlable system implementation
*/

#include <memory>
#include <raylib.h>

#include "Controlable.hpp"

#include "ECS/Components.hpp"
#include "Entity/Components/Player/Player.hpp"

#include "ECS/Zipper.hpp"

#include <iostream>

void Controlable::update(registry& r, float dt) {
    movePlayer(r);
}

void Controlable::movePlayer(registry &r)
{
    auto playerArr = r.get_if<Player>();
    auto posArr = r.get_if<position>();
    auto velArr = r.get_if<velocity>();

    for (auto [player, pos, vel, ent]: zipper(*playerArr, *posArr, *velArr)) {
        if (IsKeyDown(KEY_RIGHT)) {
            vel.vx = player._moveSpeed;
        } else if (IsKeyDown(KEY_LEFT)) {
            vel.vx = -player._moveSpeed;
        } else {
            vel.vx = 0;
        }
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<Controlable>();
    }
}
