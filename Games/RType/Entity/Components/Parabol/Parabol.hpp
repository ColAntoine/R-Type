/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Parabol component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct Parabol : public IComponent {
    float _startX{0.f};
    float _startY{0.f};
    float _range{100.f};
    bool _flickedUp{false};

    Parabol();
    Parabol(float x, float y, float range, bool down);
};
