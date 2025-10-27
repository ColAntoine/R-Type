/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PowerUp component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

#include <map>
#include <functional>

enum powerUpType {
    PLAYER_SPEED,
    WEAPON_FIRERATE,
    WAEPON_COOLDOWN,
    WEAPON_NEW,
};

struct PowerUp : public IComponent {
    powerUpType _pwType;

    PowerUp();
    PowerUp(powerUpType pwType);
};
