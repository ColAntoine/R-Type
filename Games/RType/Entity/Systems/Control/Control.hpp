/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Control System
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

class ControlSystem : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "ControlSystem"; }

private:
    void checkShoot(registry& r);
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
