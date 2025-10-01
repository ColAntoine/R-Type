/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Lifetime System
*/

#pragma once

#include "ecs/systems/isystem.hpp"

class LifetimeSystem : public ISystem {
private:
    int window_width = 800;
    int window_height = 600;

public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "LifetimeSystem"; }
    void set_window_size(int width, int height) { window_width = width; window_height = height; }
};

extern "C" {
    ISystem* create_system();
    void destroy_system(ISystem* system);
}
