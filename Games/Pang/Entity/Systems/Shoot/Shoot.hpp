/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Shoot system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"
#include <raylib.h>

#include "ECS/Zipper.hpp"
#include "ECS/Registry.hpp"
#include "ECS/ComponentFactory.hpp"
#include "ECS/Components.hpp"

#include "Entity/Components/Player/Player.hpp"
#include "Entity/Components/Rope/Rope.hpp"
#include "Entity/Components/Ball/Ball.hpp"
#include "Constants.hpp"

class Shoot : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "Shoot"; }

    void checkShoot(registry &r);
    void updateCd(registry &r, float dt);
    void updateRopes(registry &r, float dt);
    void spawnRope(registry &r, float playerX, float playerY);
    void checkBallCollision(registry &r);
    void updateScore(registry &r);
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
