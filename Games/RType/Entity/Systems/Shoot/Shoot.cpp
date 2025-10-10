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

#include "Shoot.hpp"

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
        r.emplace_component<collider>(projectile);
        r.emplace_component<lifetime>(projectile);

        weapon._cooldown = (weapon._fireRate > 0.0f) ? (1.0f / weapon._fireRate) : 1.0f;
        if (weapon._ammo > 0) --weapon._ammo;
        if (!weapon._automatic) weapon._wantsToFire = false;
    }
}

// ...existing code...
void Shoot::checkEnnemyHits(registry &r)
{
    // get arrays
    auto *projArr = r.get_if<Projectile>();
    auto *posArr = r.get_if<position>();
    auto *healthArr = r.get_if<Health>();
    auto *colArr = r.get_if<collider>(); // optional, used if targets have colliders/rects
    std::vector<entity> entityToKill;

    if (!projArr || !posArr || !healthArr) return;

    // Iterate projectiles that have a position
    for (auto [proj, ppos, projEntity] : zipper(*projArr, *posArr)) {
        float pr = proj._radius;        // projectile collision radius
        int pdmg = proj._damage;       // projectile damage
        std::size_t owner = proj._ownerId;  // projectile owner id (skip friendly fire)

        // Compute projectile collision center including projectile offset along its direction
        // (moves collision origin forward by radius so collision matches visual projectile tip)
        float pcenterX = ppos.x + proj._dirX * pr;
        float pcenterY = ppos.y + proj._dirY * pr;

        // TODO: remove
        if (pcenterX > 700.f) {
            entityToKill.push_back(entity(projEntity));
        }
        // If we have collider data for targets, iterate only entities that have Health+Position+Collider.
        // Use direct AABB extents (no half-width computation).
        if (colArr) {
            for (auto [hlt, hpos, c, targetEntity] : zipper(*healthArr, *posArr, *colArr)) {
                if (projEntity == targetEntity) continue;
                // if (targetEntity == static_cast<decltype(targetEntity)>(owner)) continue;

                // Treat collider as top-left (hpos.x, hpos.y) with width c.w and height c.h
                float left   = hpos.x;
                float right  = hpos.x + c.w;
                float top    = hpos.y;
                float bottom = hpos.y + c.h;

                // clamp projectile center to rectangle
                float closestX = std::max(left, std::min(pcenterX, right));
                float closestY = std::max(top, std::min(pcenterY, bottom));
                float dx = pcenterX - closestX;
                float dy = pcenterY - closestY;
                float dist2 = dx * dx + dy * dy;

                if (dist2 <= pr * pr) {
                    hlt._health -= pdmg;
                    std::cout << "HIT target=" << targetEntity << " dmg=" << pdmg << " hp=" << hlt._health << std::endl;
                    entityToKill.push_back(entity(projEntity));
                    break; // stop testing this projectile after a hit
                }
            }
        } else {
            // No collider component available: treat targets as points (Health+Position)
            for (auto [hlt, hpos, targetEntity] : zipper(*healthArr, *posArr)) {
                if (projEntity == targetEntity) continue;
                if (targetEntity == static_cast<decltype(targetEntity)>(owner)) continue;

                float dx = pcenterX - hpos.x;
                float dy = pcenterY - hpos.y;
                float dist2 = dx * dx + dy * dy;

                if (dist2 <= pr * pr) {
                    hlt._health -= pdmg;
                    entityToKill.push_back(entity(projEntity));
                    std::cout << "HIT point target=" << targetEntity << " dmg=" << pdmg << " hp=" << hlt._health << std::endl;
                    break;
                }
            }
        }
    }

    // remove duplicates and kill entities after finishing iteration (avoid corrupting iterators)
    if (!entityToKill.empty()) {
        std::sort(entityToKill.begin(), entityToKill.end());
        entityToKill.erase(std::unique(entityToKill.begin(), entityToKill.end()), entityToKill.end());

        for (auto ent : entityToKill) {
            r.kill_entity(ent);
        }
    }
}
// ...existing code...
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
