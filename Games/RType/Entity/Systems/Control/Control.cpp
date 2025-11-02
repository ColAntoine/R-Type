/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Control System Implementation
*/

#include <iostream>
#include <raylib.h>
#include "Entity/Components/Controllable/Controllable.hpp"
#include "Entity/Components/Input/Input.hpp"
#include "Entity/Components/Weapon/Weapon.hpp"
#include "Entity/Systems/Control/Control.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components.hpp"
#include "ECS/Zipper.hpp"
#include "Core/Client/Network/NetworkService.hpp"

void ControlSystem::update(registry& r, float dt) {
    auto *ctrl_arr = r.get_if<controllable>();
    auto *input_arr = r.get_if<Input>();
    if (!ctrl_arr || !input_arr) return;
    auto* network_manager = RType::Network::get_network_manager();
    if (network_manager) {
        return;
    }

    // Iterate all controllable entities and set/create their velocity component
    for (std::size_t i = 0; i < ctrl_arr->size(); ++i) {
        auto ent_idx = ctrl_arr->entity_at(i);
        entity ent(ent_idx);
        auto &ctrl = (*ctrl_arr)[i];
        auto &input = (*input_arr)[i];

        float speed = ctrl.speed;
        float vx = 0.f, vy = 0.f;
        if (input.move_right) vx += speed;
        if (input.move_left)  vx -= speed;
        if (input.move_down)  vy += speed;
        if (input.move_up)    vy -= speed;

        auto *vel_arr = r.get_if<velocity>();
        if (vel_arr && vel_arr->has(ent)) {
            vel_arr->get(ent) = velocity(vx, vy);
        } else {
            r.emplace_component<velocity>(ent, vx, vy);
        }
    }
}

DLL_EXPORT ISystem* create_system() {
    try {
        return new ControlSystem();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}