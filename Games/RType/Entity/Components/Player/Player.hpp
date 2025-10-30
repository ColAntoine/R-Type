/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Player component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct Player : public IComponent {
    bool _isDead{false};
    Player();
};
