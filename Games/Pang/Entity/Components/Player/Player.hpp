/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Player component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct Player : public IComponent {
    int _life{3};
    float _cooldown{0.5f};
    float _currentCooldown{0.f};
    int _score{0};
    float _invincibility;

    Player();
    Player(int life, float cooldown, float currentCooldown, int score, float invincibilty);
};
