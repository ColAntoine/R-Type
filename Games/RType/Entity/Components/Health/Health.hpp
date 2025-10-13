/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Health component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct Health : public IComponent {
    int _health{100};

    Health();
    Health(int health);
};
