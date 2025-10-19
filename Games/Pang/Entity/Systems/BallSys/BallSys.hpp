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
    bool checkPlayerColision(position &playerPos, position &ballPos, Ball &ball);

    void checkBallCollisions(registry &r);
    bool checkBallToBallCollision(position &ball1Pos, position &ball2Pos, Ball &ball1, Ball &ball2);
    void resolveBallCollision(position &ball1Pos, position &ball2Pos, Ball &ball1, Ball &ball2,
        velocity &ball1Vel, velocity &ball2Vel);
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
