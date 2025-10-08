/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Velocity Component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct velocity : public IComponent {
    float vx{};
    float vy{};

    velocity();
    velocity(float nvx, float nvy);
};
