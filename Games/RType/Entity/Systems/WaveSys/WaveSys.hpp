/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WaveSys system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

#include "ECS/Registry.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Entity.hpp"

#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/Components/Animation.hpp"

#include "Entity/Components/Wave/Wave.hpp"
#include "Entity/Components/Projectile/Projectile.hpp"
#include "Entity/Components/Weapon/Weapon.hpp"
#include "Entity/Components/LifeTime/LifeTime.hpp"

#include "Constants.hpp"

#include <iostream>
#include <vector>
#include <cmath>

class WaveSys : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "WaveSys"; }

private:
    void updateTimer(registry &r, float dt);
    void handleStop(registry &r);
    void handleExplosion(registry &r);
};

#if defined(_WIN32)
  #define DLL_EXPORT extern "C" __declspec(dllexport)
#else
  #define DLL_EXPORT extern "C"
#endif

DLL_EXPORT ISystem* create_system();
DLL_EXPORT void     destroy_system(ISystem* ptr);
