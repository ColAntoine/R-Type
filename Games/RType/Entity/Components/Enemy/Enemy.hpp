/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Enemy Component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct enemy : public IComponent {
    int enemy_type{0};  // 0=basic, 1=fast, 2=heavy, etc.
    float health{1.0f};

    enemy();
    enemy(int type, float hp = 1.0f);
};
