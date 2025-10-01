/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Position Component
*/

#pragma once

#include "ecs/components/icomponent.hpp"

struct position : public IComponent {
    float x{};
    float y{};

    position();
    position(float nx, float ny);
};
