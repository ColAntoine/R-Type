/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Lifetime System
*/

#pragma once

#include "Constants.hpp"
#include "ECS/Systems/ISystem.hpp"

class LifetimeSystem : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "LifetimeSystem"; }
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
