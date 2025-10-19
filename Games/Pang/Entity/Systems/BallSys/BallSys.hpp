/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** BallSys system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

#include "ECS/Components.hpp"
#include "Entity/Components/Ball/Ball.hpp"
#include "Entity/Components/Player/Player.hpp"
#include "Entity/Components/Invincibility/Invincibility.hpp"

class BallSys : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "BallSys"; }

private:
    void bounceBalls(registry &r);
    void checkPlayerHits(registry &r);
    void computeBallPhysics(position &ballPos, position &playerPos, Ball &ball,
        velocity &ballVel, velocity &playerVel);
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
