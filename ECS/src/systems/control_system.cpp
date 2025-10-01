/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Control System Implementation
*/

#include <raylib.h>
#include "ecs/systems/control_system.hpp"
#include "ecs/registry.hpp"
#include "ecs/components.hpp"
#include "ecs/zipper.hpp"

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
}

extern "C" ISystem* create_system() {
    return new ControlSystem();
}

extern "C" void destroy_system(ISystem* system) {
    delete system;
}
