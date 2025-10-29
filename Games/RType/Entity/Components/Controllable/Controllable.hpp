/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Controllable Component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct controllable : public IComponent {
    float speed{100.f};

    controllable();
    controllable(float s);
};
