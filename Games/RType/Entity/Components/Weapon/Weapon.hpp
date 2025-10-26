/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Weapon component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"
#include <string>

struct Weapon : public IComponent {
    int _ownerId{-1};                        // entity id that owns the weapon (optional)
    std::string _projectileType{"hardBullet"};   // identifier for which projectile to spawn
    float _fireRate{3.f};                   // shots per second
    float _cooldown{0.0f};                   // seconds until next shot (runtime)
    int _damage{10};                          // base damage applied to projectile
    float _projectileSpeed{400.0f};          // default projectile speed
    int _ammo{-1};                           // -1 = infinite
    bool _automatic{false};                  // hold to fire
    bool _wantsToFire{false};                // request to fire set by input/system
    bool _justFired{false};                  // transient flag set when projectile actually spawned

    Weapon() = default;
    Weapon(int owner, const std::string &projType, float rate, int dmg, float projSpeed, int am = -1, bool autoFire = false);
};