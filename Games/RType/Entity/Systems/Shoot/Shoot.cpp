/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Shoot system implementation
*/

#include <memory>
#include <raylib.h>
#include <iostream>
#include <algorithm>
#include <string>

#include "Shoot.hpp"

#include <raylib.h>
#include <cmath>

Shoot::Shoot()
    : _shootType()
{
    // Register shoot functions for different weapon types
    _shootType["bullet"] = [this](const ProjectileContext& ctx) {
        shoot_base_bullets(ctx);
    };
}

void Shoot::checkShootIntention(registry & r)
{
    auto *ctrl_arr = r.get_if<controllable>();
    auto *weaponArr = r.get_if<Weapon>();

    if (!ctrl_arr || !weaponArr) return;

    for (auto [ctrl, weapon, entity] : zipper(*ctrl_arr, *weaponArr)) {
        if (IsKeyDown(KEY_SPACE)) {
            weapon._wantsToFire = true;
            weapon._ownerId = entity;
        }
    }
}

void Shoot::spawnProjectiles(registry &r, float dt)
{
    auto *weaponArr = r.get_if<Weapon>();
    auto *posArr = r.get_if<position>();
    auto *colArr = r.get_if<collider>();

    if (!weaponArr || !posArr) return;

    for (auto [weapon, pos, entityId]: zipper(*weaponArr, *posArr)) {
        if (weapon._cooldown > 0.0f) {
            weapon._cooldown = std::max(0.0f, weapon._cooldown - dt);
        }

        if (!weapon._wantsToFire || weapon._cooldown != 0.0f || weapon._ammo == 0) {
            weapon._wantsToFire = false;
            continue;
        }

        float spawnX = pos.x;
        float spawnY = pos.y;

        float dirX = 1.0f;
        float dirY = 0.0f;

        if (colArr && colArr->has(static_cast<size_t>(entityId))) {
            auto &col = colArr->get(static_cast<size_t>(entityId));
            spawnX = pos.x + col.offset_x + col.w;
            spawnY = pos.y + col.offset_y + col.h / 2.0f;
        }

        ProjectileContext ctx{r, entity(entityId), weapon, spawnX, spawnY, dirX, dirY};

        auto it = _shootType.find(weapon._projectileType);
        if (it != _shootType.end()) {
            it->second(ctx);
        }

        weapon._cooldown = (weapon._fireRate > 0.0f) ? (1.0f / weapon._fireRate) : 1.0f;
        if (weapon._ammo > 0) --weapon._ammo;
        weapon._justFired = true;
        if (!weapon._automatic) weapon._wantsToFire = false;
    }
}

void Shoot::checkEnnemyHits(registry &r)
{
    auto *projArr = r.get_if<Projectile>();
    auto *posArr = r.get_if<position>();
    auto *healthArr = r.get_if<Health>();
    auto *colArr = r.get_if<collider>();
    auto *enemyArr = r.get_if<Enemy>();
    std::vector<entity> entityToKill;

    if (!projArr || !posArr || !healthArr || !enemyArr || !colArr) return;

    // Iterate projectiles that have a position
    for (auto [proj, ppos, projEntity] : zipper(*projArr, *posArr)) {
        float pr = proj._radius;        // projectile collision radius
        int pdmg = proj._damage;        // projectile damage
        std::size_t owner = proj._ownerId;  // projectile owner id (skip friendly fire)

    // Compute projectile collision center including projectile offset along its direction
    // (moves collision origin forward by radius so collision matches visual projectile tip)
        float pcenterX = ppos.x + proj._dirX * pr;
        float pcenterY = ppos.y + proj._dirY * pr;

        // If we have collider data for targets, iterate only entities that have Health+Position+Collider.
        // Use direct AABB extents (no half-width computation).
        for (auto [hlt, hpos, c, enemyEnt, targetEntity] : zipper(*healthArr, *posArr, *colArr, *enemyArr)) {
            if (projEntity == targetEntity) continue;

            // Treat collider using its offset relative to the entity position
            float left   = hpos.x + c.offset_x;
            float right  = hpos.x + c.offset_x + c.w;
            float top    = hpos.y + c.offset_y;
            float bottom = hpos.y + c.offset_y + c.h;

            float closestX = std::max(left, std::min(pcenterX, right));
            float closestY = std::max(top, std::min(pcenterY, bottom));
            float dx = pcenterX - closestX;
            float dy = pcenterY - closestY;
            float dist2 = dx * dx + dy * dy;

            if (dist2 <= pr * pr) {
                hlt._health -= pdmg;
                entityToKill.push_back(entity(projEntity));
                break;
            }
        }
    }

    // remove duplicates and kill entities after finishing iteration (avoid corrupting iterators)
    if (!entityToKill.empty()) {
        std::sort(entityToKill.begin(), entityToKill.end());
        entityToKill.erase(std::unique(entityToKill.begin(), entityToKill.end()), entityToKill.end());

        for (auto ent : entityToKill) {
            // Only kill if the entity still exists
            if (r.get_if<position>() && r.get_if<position>()->has(static_cast<size_t>(ent))) {
                r.kill_entity(ent);
            }
        }
    }
}

void Shoot::shoot_base_bullets(const ProjectileContext& ctx)
{
    auto projectile = ctx.r.spawn_entity();

    ctx.r.emplace_component<Projectile>(projectile, Projectile(ctx.owner_entity, ctx.weapon._damage, ctx.weapon._projectileSpeed, ctx.dir_x, ctx.dir_y, 5.0f, 4.0f, true));
    ctx.r.emplace_component<position>(projectile, ctx.spawn_x, ctx.spawn_y);
    ctx.r.emplace_component<velocity>(projectile, ctx.dir_x * ctx.weapon._projectileSpeed, ctx.dir_y * ctx.weapon._projectileSpeed);
    ctx.r.emplace_component<animation>(projectile, std::string(RTYPE_PATH_ASSETS) + "Binary_bullet-Sheet.png", 220, 220, 0.1f, 0.1f, 0, false);
    ctx.r.emplace_component<lifetime>(projectile);
    ctx.r.emplace_component<Gravity>(projectile, 100.0f);
}

void Shoot::update(registry& r, float dt) {
    checkShootIntention(r);
    spawnProjectiles(r, dt);
    checkEnnemyHits(r);
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<Shoot>();
    }
}
