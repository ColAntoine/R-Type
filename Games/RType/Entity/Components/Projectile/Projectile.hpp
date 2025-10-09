/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Projectile component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct Projectile : public IComponent {
    int _ownerId{-1};      // entity id that fired this projectile
    int _damage{1};        // damage on hit
    float _speed{400.0f};  // scalar speed (units / second)
    float _dirX{0.0f};     // normalized direction X
    float _dirY{0.0f};     // normalized direction Y
    float _lifetime{5.0f}; // seconds before auto-despawn
    float _radius{4.0f};   // collision radius
    bool _friendly{true};  // whether it should hit enemies or players

    Projectile() = default;
    Projectile(int owner, int dmg, float spd, float dx, float dy, float life = 5.0f, float rad = 4.0f, bool friendly = true);
};
