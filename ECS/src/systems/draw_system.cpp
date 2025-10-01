/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Draw System Implementation
*/

#include <raylib.h>
#include "ecs/systems/draw_system.hpp"
#include "ecs/registry.hpp"
#include "ecs/components.hpp"
#include "ecs/zipper.hpp"

void DrawSystem::update(registry& r, float dt) {
    auto *pos_arr = r.get_if<position>();
    auto *draw_arr = r.get_if<drawable>();
    if (!pos_arr || !draw_arr) return;

    for (auto [p, d, entity] : zipper(*pos_arr, *draw_arr)) {
        DrawRectangle((int)p.x, (int)p.y, (int)d.w, (int)d.h,
                     (Color){d.r, d.g, d.b, d.a});
    }
}

extern "C" ISystem* create_system() {
    return new DrawSystem();
}

extern "C" void destroy_system(ISystem* system) {
    delete system;
}
