#include <raylib.h>
#include "Draw.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components.hpp"
#include "ECS/Zipper.hpp"
#include "Entity/Components/Drawable/Drawable.hpp"
#include <iostream>

void DrawSystem::update(registry& r, float dt) {
    std::cout << "CHECK WINDOW READINESS" << std::endl;
    if (!IsWindowReady()) {
        std::cout << "WINDOW NOT READY" << std::endl;
        return;
    }

    auto *pos_arr = r.get_if<position>();
    auto *draw_arr = r.get_if<drawable>();

    if (!pos_arr || !draw_arr) return;

    for (auto [p, d, entity] : zipper(*pos_arr, *draw_arr)) {
        DrawRectangle((int)p.x, (int)p.y, (int)d.w, (int)d.h, RED);
    }
}

std::unique_ptr<ISystem> create_system() {
    return std::make_unique<DrawSystem>();
}