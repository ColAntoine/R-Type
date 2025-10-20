/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Rope component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"
#include <raylib.h>

struct Rope : public IComponent {
    float _width{3.f};           // Width of the rope visual
    float _speed{800.f};         // Speed of rope traveling upward
    bool _isActive{true};        // Whether the rope is active
    float _startY{0.f};          // Starting Y position (player position)
    float _currentTipY{0.f};     // Current tip position (grows upward)
    Color _color{WHITE};         // Rope color

    Rope();
    Rope(float width, float speed, float startY, Color color);
};
