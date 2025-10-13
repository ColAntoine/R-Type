/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Enemy Component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"
#include <cstdint>

enum EnemyAIType : uint8_t {
    BASIC = 1,
    SINE_WAVE = 2,
    FAST = 3,
    ZIGZAG = 4
};

struct enemy : public IComponent {
    EnemyAIType enemy_type{BASIC};
    float timer{0.0f};
    float movement_pattern{0.0f};

    enemy() = default;
    enemy(EnemyAIType type);
    enemy(uint8_t type);
};
