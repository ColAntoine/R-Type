/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Position Component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct position : public IComponent {
    float x{0.f};
    float y{0.f};

    position();
    position(float nx, float ny);
};
