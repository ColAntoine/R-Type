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

    bool move_up = false;
    bool move_down = false;
    bool move_left = false;
    bool move_right = false;
    bool shoot = false;

    controllable();
    controllable(float s);
};
