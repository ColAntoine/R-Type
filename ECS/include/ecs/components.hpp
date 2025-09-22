/*
** EPITECH PROJECT, 2025
** asd
** File description:
** asd
*/

#pragma once

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

extern "C" {
void register_components(registry &r);
}
