/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PUpAnimation component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

#include <string>

struct PUpAnimation : public IComponent {
    bool _up{true};
    std::string _text{""};
    float _duration{3.0f};
    float _elapsed{0.0f};

    PUpAnimation();
    PUpAnimation(bool up, std::string text);
    PUpAnimation(bool up, std::string text, float duration);
};
