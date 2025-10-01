/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Draw System
*/

#pragma once

#include "ecs/systems/isystem.hpp"

class DrawSystem : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "DrawSystem"; }
};

extern "C" {
    ISystem* create_system();
    void destroy_system(ISystem* system);
}
