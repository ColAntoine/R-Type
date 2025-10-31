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

#include "ECS/Registry.hpp"

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
    void splitBalls(registry &r);
};

#if defined(_WIN32)
  #define DLL_EXPORT extern "C" __declspec(dllexport)
#else
  #define DLL_EXPORT extern "C"
#endif

DLL_EXPORT ISystem* create_system();
DLL_EXPORT void     destroy_system(ISystem* ptr);