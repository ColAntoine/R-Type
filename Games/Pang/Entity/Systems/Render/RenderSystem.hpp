/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** RenderSystem header
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

class RenderSystem : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "RenderSystem"; }

private:
    void renderPlayers(registry &r);
    void renderBalls(registry &r);
    void renderRopes(registry &r);
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
