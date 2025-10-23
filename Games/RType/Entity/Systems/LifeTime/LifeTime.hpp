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
private:
    int window_width = SCREEN_WIDTH;
    int window_height = SCREEN_HEIGHT;

public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "LifetimeSystem"; }
    void set_window_size(int width, int height) { window_width = width; window_height = height; }
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
