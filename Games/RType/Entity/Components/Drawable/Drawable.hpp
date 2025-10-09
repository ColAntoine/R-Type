/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Drawable Component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct drawable : public IComponent {
    unsigned char r{255}, g{255}, b{255}, a{255};
    float w{10.f}, h{10.f};

    drawable();
    drawable(float nw, float nh, unsigned char nr = 255, unsigned char ng = 255,
            unsigned char nb = 255, unsigned char na = 255);
};
