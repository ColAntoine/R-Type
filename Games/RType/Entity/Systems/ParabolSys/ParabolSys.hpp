/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ParabolSys system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Zipper.hpp"

#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"

#include "Entity/Components/Gravity/Gravity.hpp"
#include "Entity/Components/Parabol/Parabol.hpp"

class ParabolSys : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "ParabolSys"; }

private:
    void handleParabols(registry &r);
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
