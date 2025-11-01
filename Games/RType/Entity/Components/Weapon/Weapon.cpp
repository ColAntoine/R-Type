/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Weapon component implementation
*/

#include "Weapon.hpp"

Weapon::Weapon(int owner, const std::string &projType, float rate, int dmg, float projSpeed, int am, bool autoFire)
: _ownerId(owner),
_projectileType{projType},
_fireRate(rate),
_damage(dmg),
_projectileSpeed(projSpeed),
_ammo(am),
_automatic(autoFire)
{

}

Weapon::Weapon(int owner, std::vector<std::string> &projType, float rate, int dmg, float projSpeed, int am, bool autoFire)
: _ownerId(owner),
_projectileType(projType),
_fireRate(rate),
_damage(dmg),
_projectileSpeed(projSpeed),
_ammo(am),
_automatic(autoFire)
{

}
