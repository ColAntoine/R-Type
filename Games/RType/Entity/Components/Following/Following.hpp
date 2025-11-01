/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Following component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct Following : public IComponent {
    float _followForce{200.f};

    Following();
    Following(float followForce);
};
