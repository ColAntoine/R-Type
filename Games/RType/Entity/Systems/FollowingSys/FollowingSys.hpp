/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** FollowingSys system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"

#include "Entity/Components/Following/Following.hpp"
#include "Entity/Components/Player/Player.hpp"

#include "ECS/Zipper.hpp"
#include "ECS/Registry.hpp"

class FollowingSys : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "FollowingSys"; }

private:
    void updatePlayerPos(registry &r);
    void updateProjTrajectories(registry  &r);

    position _playerPos;
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
