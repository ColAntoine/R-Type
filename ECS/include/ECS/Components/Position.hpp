/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Position Component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct position : public IComponent {
    float x{};
    float y{};

    position();
    position(float nx, float ny);
};
