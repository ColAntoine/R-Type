/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Projectile component implementation
*/

#include <cmath>

#include "Projectile.hpp"

Projectile::Projectile() {}
Projectile::Projectile(int owner, int dmg, float spd, float dx, float dy, float life, float rad, bool isFriendly)
: _ownerId(owner),
_damage(dmg),
_speed(spd),
_dirX(dx),
_dirY(dy),
_lifetime(life),
_radius(rad),
_friendly(isFriendly)
{
    // Optionally normalize direction if not zero
    float len = std::sqrt(_dirX * _dirX + _dirY * _dirY);
    if (len > 0.0f) {
        _dirX /= len;
        _dirY /= len;
    } else {
        _dirX = 1.0f; // default direction if none provided
        _dirY = 0.0f;
    }
}
