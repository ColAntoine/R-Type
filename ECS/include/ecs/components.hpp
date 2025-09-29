/*
** EPITECH PROJECT, 2025
** asd
** File description:
** asd
*/

#pragma once

#include <string>
#include "ecs/registry.hpp"
#include "ecs/icomponent.hpp"

struct position : public IComponent {
    float x{};
    float y{};

    position();
    position(float nx, float ny);
};

struct velocity : public IComponent {
    float vx{};
    float vy{};

    velocity();
    velocity(float nvx, float nvy);
};

struct drawable : public IComponent {
    unsigned char r{255}, g{255}, b{255}, a{255};
    float w{10.f}, h{10.f};

    drawable();
    drawable(float nw, float nh, unsigned char nr = 255, unsigned char ng = 255,
             unsigned char nb = 255, unsigned char na = 255);
};

struct controllable : public IComponent {
    float speed{100.f};
    controllable();
    controllable(float s);
};

struct collider : public IComponent {
    float w{10.f}, h{10.f};
    float offset_x{0.f}, offset_y{0.f};
    bool is_trigger{false};

    collider();
    collider(float nw, float nh, float ox = 0.f, float oy = 0.f, bool trig = false);
};

struct remote_player : public IComponent {
    std::string client_id;

    remote_player();
    remote_player(const std::string& id);
};

struct enemy : public IComponent {
    int enemy_type{0};  // 0=basic, 1=fast, 2=heavy, etc.
    float health{1.0f};

    enemy();
    enemy(int type, float hp = 1.0f);
};

struct lifetime : public IComponent {
    float max_lifetime{10.0f};  // Maximum time to live (seconds)
    float current_time{0.0f};   // Current age
    bool destroy_on_collision{false};  // Whether to destroy on collision
    bool destroy_offscreen{true};      // Whether to destroy when offscreen

    lifetime();
    lifetime(float max_time, bool collision_destroy = false, bool offscreen_destroy = true);
};

struct spawner : public IComponent {
    float spawn_interval{2.0f};   // Time between spawns (seconds)
    float last_spawn_time{0.0f};  // Time since last spawn
    int max_entities{10};         // Maximum concurrent entities
    int current_count{0};         // Current spawned count

    spawner();
    spawner(float interval, int max_ents = 10);
};

extern "C" {
void register_components(registry &r);
}
