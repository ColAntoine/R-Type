/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Gravity component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

static constexpr float DEFAULT_GRAVITY = 600.0f;

struct Gravity : public IComponent {
    float _force{DEFAULT_GRAVITY};

    Gravity();
    Gravity(float force=DEFAULT_GRAVITY);
};
