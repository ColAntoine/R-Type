#include <iostream>
#include <algorithm>
#include <raylib.h>

#include "ecs/registry.hpp"
#include "ecs/systems.hpp"
#include "ecs/zipper.hpp"
#include "ecs/component_factory.hpp"
#include "ecs/rtype.hpp"

RType::RType()
{
    InitWindow(800, 600, "ECS Raylib Demo");
    SetTargetFPS(60);

    if (!loader_.load_components_from_so("./libcomponents.so", reg_)) {
        std::cerr << "Failed to load components library!" << std::endl;
        exit(EXIT_FAILURE);
    }
    setup_scene();
}

int RType::run() {
    while (!WindowShouldClose()) {
        handle_events();
        float dt = GetFrameTime();
        update(dt);
        render();
    }
    CloseWindow();
    return 0;
}

void RType::setup_scene() {
    auto factory = loader_.get_factory();
    if (!factory) {
        std::cerr << "Factory not loaded!" << std::endl;
        return;
    }

    create_player(factory, 100.f, 100.f, 40.f, 40.f, 300.f);

    create_static_entity(factory, 200.f, 150.f, 60.f, 60.f, 100, 255, 100);
    create_static_entity(factory, 400.f, 300.f, 80.f, 30.f, 255, 200, 50);
    create_static_entity(factory, 600.f, 450.f, 50.f, 100.f, 120, 180, 240, true);

    create_moving_entity(factory, 50.f, 500.f, 30.f, 30.f, 80.f, 0.f, 255, 255, 0);
}

void RType::create_player(IComponentFactory* factory, float x, float y, float w, float h, float speed) {
    auto entity = reg_.spawn_entity();
    factory->create_position(reg_, entity, x, y);
    factory->create_velocity(reg_, entity, 0.f, 0.f);
    factory->create_drawable(reg_, entity, w, h, 200, 100, 100, 255);
    factory->create_controllable(reg_, entity, speed);
    factory->create_collider(reg_, entity, w, h, 0.f, 0.f, false);
}

void RType::create_static_entity(IComponentFactory* factory, float x, float y, float w, float h,
                                uint8_t r, uint8_t g, uint8_t b, bool has_collider) {
    auto entity = reg_.spawn_entity();
    factory->create_position(reg_, entity, x, y);
    factory->create_drawable(reg_, entity, w, h, r, g, b, 255);
    if (has_collider) {
        factory->create_collider(reg_, entity, w, h, 0.f, 0.f, false);
    }
}

void RType::create_moving_entity(IComponentFactory* factory, float x, float y, float w, float h,
                               float vx, float vy, uint8_t r, uint8_t g, uint8_t b) {
    auto entity = reg_.spawn_entity();
    factory->create_position(reg_, entity, x, y);
    factory->create_velocity(reg_, entity, vx, vy);
    factory->create_drawable(reg_, entity, w, h, r, g, b, 255);
    factory->create_collider(reg_, entity, w, h, 0.f, 0.f, false);
}

void RType::handle_events() {
    // Event handling is done by Raylib's WindowShouldClose() in the main loop
    // Additional input handling can be added here if needed
}

void RType::update(float dt) {
    control_system(reg_);
    // Time-aware movement: iterate position/velocity and apply dt
    position_system(reg_, dt);
    collision_system(reg_);
}

void RType::render() {
    BeginDrawing();
    ClearBackground(BLACK);
    draw_system(reg_);
    EndDrawing();
}

// int main() {
//     RType app;
//     return app.run();
// }
