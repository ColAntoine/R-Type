/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Shoot system implementation
*/

#include <memory>
#include <raylib.h>
#include <iostream>

#include "Shoot.hpp"

static void checkShootIntention(registry & r)
{
    auto *ctrl_arr = r.get_if<controllable>();
    auto *weaponArr = r.get_if<Weapon>();

    if (!ctrl_arr || !weaponArr) return;

    for (auto [ctrl, weapon, entity] : zipper(*ctrl_arr, *weaponArr)) {
        if (IsKeyDown(KEY_SPACE)) {
            std::cout << "Wants to shoot" << std::endl;
            weapon._wantsToFire = true;
            weapon._ownerId = entity;
        }
    }
}

static void spawnProjectiles(registry &r, float dt)
{
    auto *weaponArr = r.get_if<Weapon>();
    auto *posArr = r.get_if<position>();

    if (!weaponArr || !posArr) return;

    for (auto [weapon, pos, entity]: zipper(*weaponArr, *posArr)) {
        if (weapon._cooldown > 0.0f) {
            weapon._cooldown = std::max(0.0f, weapon._cooldown - dt);
        }

        if (!weapon._wantsToFire || weapon._cooldown != 0.0f || weapon._ammo == 0) {
            weapon._wantsToFire = false;
            continue;
        }

        float spawnX = pos.x;
        float spawnY = pos.y;

        auto projectile = r.spawn_entity();

        float dirX = 1.0f;
        float dirY = 0.0f;

        float life = 5.0f;
        float radius = 4.0f;

        r.emplace_component<Projectile>(projectile, Projectile(entity, weapon._damage, weapon._projectileSpeed, dirX, dirY, life, radius, true));
        r.emplace_component<position>(projectile, spawnX, spawnY);
        r.emplace_component<velocity>(projectile, dirX * weapon._projectileSpeed, dirY * weapon._projectileSpeed);
        r.emplace_component<drawable>(projectile);

        weapon._cooldown = (weapon._fireRate > 0.0f) ? (1.0f / weapon._fireRate) : 1.0f;
        if (weapon._ammo > 0) --weapon._ammo;
        if (!weapon._automatic) weapon._wantsToFire = false;
    }
}

void Shoot::update(registry& r, float dt) {
    checkShootIntention(r);
    spawnProjectiles(r, dt);
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<Shoot>();
    }
}
