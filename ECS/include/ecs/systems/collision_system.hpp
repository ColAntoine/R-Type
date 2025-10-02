/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Collision System
*/

#pragma once

#include "ecs/systems/isystem.hpp"

class CollisionSystem : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "CollisionSystem"; }
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
