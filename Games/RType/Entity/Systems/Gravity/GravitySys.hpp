/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GravitySys system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

class GravitySys : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "GravitySys"; }
private:
    void applyGravitySys(registry &r, float dt);
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
