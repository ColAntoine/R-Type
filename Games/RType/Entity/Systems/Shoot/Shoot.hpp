/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Shoot system
*/

#pragma once

#include "Entity/Components/Weapon/Weapon.hpp"
#include "Entity/Components/LifeTime/LifeTime.hpp"
#include "Entity/Components/Controllable/Controllable.hpp"
#include "Entity/Components/Projectile/Projectile.hpp"
#include "Entity/Components/Drawable/Drawable.hpp"

#include "ECS/Systems/ISystem.hpp"

#include "ECS/Registry.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Components.hpp"


class Shoot : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "Shoot"; }
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
