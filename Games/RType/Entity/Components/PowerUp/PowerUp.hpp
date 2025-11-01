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
    HEALTH_UP,
    WEAPON_DAMAGE,
};

struct PowerUp : public IComponent {
    powerUpType _pwType;
    float _w{10.f}, _h{10.f};
    float _offset_x{0.f}, _offset_y{0.f};

    PowerUp();
    PowerUp(powerUpType pwType, float w, float h, float ox, float oy);
};
