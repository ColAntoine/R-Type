#include <iostream>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <dlfcn.h>

#include "ecs/registry.hpp"
#include "ecs/demo_app.hpp"
#include "ecs/systems.hpp"
#include "ecs/zipper.hpp"
#include "ecs/component_factory.hpp"

using RegisterComponentsFunc = void (*)(registry &);
using GetFactoryFunc = IComponentFactory* (*)();

static IComponentFactory* g_factory = nullptr;

void load_components_from_so(const std::string &so_path, registry &reg) {
    void *handle = dlopen(so_path.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load shared library: " << dlerror() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Load the register_components function
    auto register_components = (RegisterComponentsFunc)dlsym(handle, "register_components");
    if (!register_components) {
        std::cerr << "Failed to find register_components: " << dlerror() << std::endl;
        dlclose(handle);
        exit(EXIT_FAILURE);
    }

    // Load the factory getter function
    auto get_factory = (GetFactoryFunc)dlsym(handle, "get_component_factory");
    if (!get_factory) {
        std::cerr << "Failed to find get_component_factory: " << dlerror() << std::endl;
        dlclose(handle);
        exit(EXIT_FAILURE);
    }

    // Call the function to register components
    register_components(reg);

    // Get the factory instance
    g_factory = get_factory();
}

DemoApp::DemoApp()
: window_{sf::VideoMode(800, 600), "ECS SFML Demo"}
{
    window_.setFramerateLimit(60);

    load_components_from_so("./libcomponents.so", reg_);
    setup_scene();
}

int DemoApp::run() {
    sf::Clock clock;
    while (window_.isOpen()) {
        handle_events();
        float dt = clock.restart().asSeconds();
        update(dt);
        render();
    }
    return 0;
}

void DemoApp::setup_scene() {
    if (!g_factory) {
        std::cerr << "Factory not loaded!" << std::endl;
        return;
    }

    auto e = reg_.spawn_entity();
    g_factory->create_position(reg_, e, 100.f, 100.f);
    g_factory->create_velocity(reg_, e, 0.f, 0.f);
    g_factory->create_drawable(reg_, e, 40.f, 40.f, 200, 100, 100, 255);
    g_factory->create_controllable(reg_, e, 300.f);
    g_factory->create_collider(reg_, e, 40.f, 40.f, 0.f, 0.f, false);

    auto s1 = reg_.spawn_entity();
    g_factory->create_position(reg_, s1, 200.f, 150.f);
    g_factory->create_drawable(reg_, s1, 60.f, 60.f, 100, 255, 100, 255);

    auto s2 = reg_.spawn_entity();
    g_factory->create_position(reg_, s2, 400.f, 300.f);
    g_factory->create_drawable(reg_, s2, 80.f, 30.f, 255, 200, 50, 255);

    auto s3 = reg_.spawn_entity();
    g_factory->create_position(reg_, s3, 600.f, 450.f);
    g_factory->create_drawable(reg_, s3, 50.f, 100.f, 120, 180, 240, 255);
    g_factory->create_collider(reg_, s3, 50.f, 100.f, 0.f, 0.f, false);

    auto mover = reg_.spawn_entity();
    g_factory->create_position(reg_, mover, 50.f, 500.f);
    g_factory->create_velocity(reg_, mover, 80.f, 0.f);
    g_factory->create_drawable(reg_, mover, 30.f, 30.f, 255, 255, 0, 255);
    g_factory->create_collider(reg_, mover, 30.f, 30.f, 0.f, 0.f, false);
}

void DemoApp::handle_events() {
    sf::Event event;
    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window_.close();
    }
}

void DemoApp::update(float dt) {
    control_system(reg_);
    // Time-aware movement: iterate position/velocity and apply dt
    if (auto *pos_arr = reg_.get_if<position>()) {
        if (auto *vel_arr = reg_.get_if<velocity>()) {
            for (auto [pos, vel, entity] : zipper(*pos_arr, *vel_arr)) {
                pos.x += vel.vx * dt;
                pos.y += vel.vy * dt;
            }
        }
    }
    collision_system(reg_);
}

void DemoApp::render() {
    window_.clear(sf::Color::Black);
    draw_system(reg_, window_);
    window_.display();
}

int main() {
    DemoApp app;
    return app.run();
}
