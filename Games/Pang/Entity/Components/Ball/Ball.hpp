/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Ball component
*/

#pragma once

#include <raylib.h>

#include "ECS/ILoader.hpp"
#include "Constants.hpp"
#include "ECS/Components/Position.hpp"

enum ballType {
    SMALL,
    MEDIUM,
    LARGE,
    CUSTOM
};

struct Ball {
    float _radius{2.f};
    Color _color{255, 255, 255, 255};
    bool _isActive{true};
    ballType _type{MEDIUM};
    bool _isHit{false};

    Ball();
    Ball(float radius, Color color, bool isACtive, ballType type);

    public:
        void spawn(IComponentFactory *factory, registry &r, position pos, ballType type=MEDIUM);
        void spawn(IComponentFactory *factory, registry &r, position pos, ballType type, float vx, float vy);
};
