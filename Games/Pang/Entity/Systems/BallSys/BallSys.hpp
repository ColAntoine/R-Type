/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** BallSys system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

class BallSys : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "BallSys"; }

private:
    void bounceBalls(registry &r);
    void checkPlayerHits(registry &r);
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
