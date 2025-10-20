/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Shoot system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"
#include <raylib.h>

#include "ECS/Zipper.hpp"
#include "ECS/Registry.hpp"

#include "Entity/Components/Player/Player.hpp"

class Shoot : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "Shoot"; }

    void checkShoot(registry &r);
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
