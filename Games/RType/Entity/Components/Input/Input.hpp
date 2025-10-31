/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Input Component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct Input : public IComponent {
    bool move_up = false;
    bool move_down = false;
    bool move_left = false;
    bool move_right = false;
    bool shoot = false;

    Input();
};
