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
    float _width{3.f};        // Width of the rope visual
    float _speed{800.f};      // Speed of rope traveling upward
    bool _isActive{true};     // Whether the rope is active
    float _maxHeight{0.f};    // Top of the screen
    Color _color{WHITE};      // Rope color

    Rope();
    Rope(float width, float speed, float maxHeight, Color color);
};
