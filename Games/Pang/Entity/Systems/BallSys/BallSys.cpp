/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** BallSys system implementation
*/

#include <memory>

#include "BallSys.hpp"

#include "ECS/Components.hpp"
#include "Entity/Components/Ball/Ball.hpp"

#include "ECS/Zipper.hpp"
#include "Constants.hpp"

void BallSys::update(registry& r, float dt) {
    bounceBalls(r);
}

void BallSys::bounceBalls(registry &r)
{
    auto ballArr = r.get_if<Ball>();
    auto velArr = r.get_if<velocity>();
    auto posArr = r.get_if<position>();

    if (!ballArr || !velArr || !posArr) return;

    for (auto [ball, vel, pos, ent]: zipper(*ballArr, *velArr, *posArr)) {
        if (pos.y >= SCREEN_HEIGHT - ball._radius) {
            pos.y = SCREEN_HEIGHT - ball._radius;
            vel.vy = -std::abs(vel.vy);
        }
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<BallSys>();
    }
}
