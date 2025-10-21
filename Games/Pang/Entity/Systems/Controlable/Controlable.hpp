/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Controlable system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

class Controlable : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "Controlable"; }
private:
    void movePlayer(registry &r);
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
