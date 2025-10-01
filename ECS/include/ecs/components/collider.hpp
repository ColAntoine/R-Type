/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Collider Component
*/

#pragma once

#include "ecs/icomponent.hpp"

struct collider : public IComponent {
    float w{10.f}, h{10.f};
    float offset_x{0.f}, offset_y{0.f};
    bool is_trigger{false};

    collider();
    collider(float nw, float nh, float ox = 0.f, float oy = 0.f, bool trig = false);
};
