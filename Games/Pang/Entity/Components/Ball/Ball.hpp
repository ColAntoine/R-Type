/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Ball component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"
#include <raylib.h>

struct Ball : public IComponent {
    float _radius{2.f};
    Color _color{Color(255, 255, 255, 255)};
    bool _isActive{true};

    Ball();
    Ball(float radius, Color color, bool isACtive);
};
