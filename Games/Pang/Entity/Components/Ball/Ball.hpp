/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Ball component
*/

#pragma once

#include <raylib.h>

#include "ECS/DLLoader.hpp"
#include "Constants.hpp"
#include "ECS/Components/Position.hpp"

struct Ball {
    float _radius{2.f};
    Color _color{Color(255, 255, 255, 255)};
    bool _isActive{true};

    Ball();
    Ball(float radius, Color color, bool isACtive);

    public:
        void spawn(IComponentFactory *factory, registry &r, position pos);
};
