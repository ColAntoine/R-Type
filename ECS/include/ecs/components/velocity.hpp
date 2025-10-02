/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Velocity Component
*/

#pragma once

#include "ecs/components/icomponent.hpp"

struct velocity : public IComponent {
    float vx{};
    float vy{};

    velocity();
    velocity(float nvx, float nvy);
};
