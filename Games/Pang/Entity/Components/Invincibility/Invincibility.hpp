/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Invincibility component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct Invincibility : public IComponent {
    float _duration{2.f};
    bool _isInvincible{false};
    float _lastActivation{0.f};

    Invincibility();
    Invincibility(float duration, bool isInvisible, float lastActivation);
};
