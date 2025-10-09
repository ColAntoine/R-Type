/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Control System Implementation
*/

#include <iostream>
#include <raylib.h>
#include "Entity/Components/Controllable/Controllable.hpp"
#include "Entity/Components/Weapon/Weapon.hpp"
#include "Entity/Systems/Control/Control.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components.hpp"
#include "ECS/Zipper.hpp"

void ControlSystem::update(registry& r, float dt) {
    auto *vel_arr = r.get_if<velocity>();
    auto *ctrl_arr = r.get_if<controllable>();
    if (!vel_arr || !ctrl_arr) return;

    for (auto [vel, ctrl, entity] : zipper(*vel_arr, *ctrl_arr)) {
        float speed = ctrl.speed;
        float vx = 0.f, vy = 0.f;
        if (IsKeyDown(KEY_RIGHT)) vx += speed;
        if (IsKeyDown(KEY_LEFT))  vx -= speed;
        if (IsKeyDown(KEY_DOWN))  vy += speed;
        if (IsKeyDown(KEY_UP))    vy -= speed;
        vel.vx = vx;
        vel.vy = vy;
    }
    checkShoot(r);
}

void ControlSystem::checkShoot(registry &r)
{
    auto *weaponArr = r.get_if<Weapon>();

    if (!weaponArr) return;

    for (auto [weapon, entity] : zipper(*weaponArr)) {
        if (IsKeyDown(KEY_SPACE)) {
            weapon._wantsToFire = true;
            std::cout << "want to shoot" << std::endl;
        }
    }
}

std::unique_ptr<ISystem> create_system() {
    return std::make_unique<ControlSystem>();
}
