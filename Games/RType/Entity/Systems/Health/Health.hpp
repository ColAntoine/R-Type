/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Health system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

#include "Components/Health/Health.hpp"
#include "Components/Score/Score.hpp"
#include "Components/Controllable/Controllable.hpp"
#include "Components/Player/Player.hpp"

#include "ECS/Registry.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Components.hpp"

class HealthSys : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "Health"; }
private:
    void checkAndKillEnemy(registry &r);
    void checkAndKillPlayer(registry &r);
    void addScore(registry &r);
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
