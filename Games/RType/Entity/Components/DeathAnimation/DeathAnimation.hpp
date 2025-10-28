/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** DeathAnimation component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

#include <string>

struct DeathAnimation : public IComponent {
    std::string _spritePath{""};
    float _duration{3.0f};
    float _elapsed{0.0f};

    DeathAnimation();
    DeathAnimation(std::string spritePath, float duration, float elapsed);
};
