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
    auto *ctrl_arr = r.get_if<controllable>();
    if (!ctrl_arr) return;

    // Iterate all controllable entities and set/create their velocity component
    for (std::size_t i = 0; i < ctrl_arr->size(); ++i) {
        auto ent_idx = ctrl_arr->entity_at(i);
        entity ent(ent_idx);
        auto &ctrl = (*ctrl_arr)[i];

        float speed = ctrl.speed;
        float vx = 0.f, vy = 0.f;
        if (IsKeyDown(KEY_RIGHT)) vx += speed;
        if (IsKeyDown(KEY_LEFT))  vx -= speed;
        if (IsKeyDown(KEY_DOWN))  vy += speed;
        if (IsKeyDown(KEY_UP))    vy -= speed;

        auto *vel_arr = r.get_if<velocity>();
        if (vel_arr && vel_arr->has(ent)) {
            vel_arr->get(ent) = velocity(vx, vy);
        } else {
            r.emplace_component<velocity>(ent, vx, vy);
        }
    }
}

std::unique_ptr<ISystem> create_system() {
    return std::make_unique<ControlSystem>();
}
