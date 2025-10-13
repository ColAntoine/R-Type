/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Draw System Implementation
*/

#include <raylib.h>
#include "Draw.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components.hpp"
#include "ECS/Zipper.hpp"

#include "Entity/Components/Drawable/Drawable.hpp"

void DrawSystem::update(registry& r, float dt) {
    auto *pos_arr = r.get_if<position>();
    auto *draw_arr = r.get_if<drawable>();
    if (!pos_arr || !draw_arr) return;

    for (auto [p, d, entity] : zipper(*pos_arr, *draw_arr)) {
        DrawRectangle((int)p.x, (int)p.y, (int)d.w, (int)d.h,
                     (Color){d.r, d.g, d.b, d.a});
    }
}

std::unique_ptr<ISystem> create_system() {
    return std::make_unique<DrawSystem>();
}
