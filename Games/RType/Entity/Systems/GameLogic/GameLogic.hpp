/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameLogic system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

#include "Entity/Components/Score/Score.hpp"
#include "Entity/Components/Enemy/Enemy.hpp"
#include "Entity/Components/Health/Health.hpp"
#include "Entity/Components/Boss/Boss.hpp"

#include "ECS/Registry.hpp"
#include "ECS/Zipper.hpp"

#include <iostream>

class GameLogic : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "GameLogic"; }
private:
    void updateScore(registry &r);
    void updateState(registry &r);

    /* Boss spawn */
    void killAllEnemy(registry &r);
    void spawnBoss(registry &r);

    Score _gameScore{0};
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
