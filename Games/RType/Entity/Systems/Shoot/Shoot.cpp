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
#include "ECS/Renderer/RenderManager.hpp"

Shoot::Shoot()
    : _shootType()
{
    _shootType["bullet"] = [this](const ProjectileContext& ctx) { shootBaseBullets(ctx); };
    _shootType["hardBullet"] = [this](const ProjectileContext& ctx) { shootHardBullets(ctx); };
    _shootType["bigBullet"] = [this](const ProjectileContext& ctx) { shootBigBullets(ctx); };
    _shootType["parabol"] = [this](const ProjectileContext& ctx) { shootParabolBullets(ctx); };
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

        // Shoot all weapon types in the vector
        for (const auto& projType : weapon._projectileType) {
            ProjectileContext ctx{r, entity(entityId), weapon, spawnX, spawnY, dirX, dirY};

            auto it = _shootType.find(projType);
            if (it != _shootType.end()) {
                it->second(ctx);
            }
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

        // If we have collider data for targets, iterate only entities that have Health+Position+Collider.
        // Use direct AABB extents (no half-width computation).
        for (auto [hlt, hpos, c, enemyEnt, targetEntity] : zipper(*healthArr, *posArr, *colArr, *enemyArr)) {
            if (projEntity == targetEntity) continue;

            // Treat collider using its offset relative to the entity position
            float left   = hpos.x + c.offset_x;
            float right  = hpos.x + c.offset_x + c.w;
            float top    = hpos.y + c.offset_y;
            float bottom = hpos.y + c.offset_y + c.h;

            float closestX = std::max(left, std::min(ppos.x, right));
            float closestY = std::max(top, std::min(ppos.y, bottom));
            float dx = ppos.x - closestX;
            float dy = ppos.y - closestY;
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

void Shoot::shootBaseBullets(const ProjectileContext& ctx)
{
    auto projectile = ctx.r.spawn_entity();

    ctx.r.emplace_component<Projectile>(projectile, Projectile(ctx.owner_entity, ctx.weapon._damage, ctx.weapon._projectileSpeed, ctx.dir_x, ctx.dir_y, 5.0f, 4.0f, true));
    ctx.r.emplace_component<position>(projectile, ctx.spawn_x, ctx.spawn_y);
    ctx.r.emplace_component<velocity>(projectile, ctx.dir_x * ctx.weapon._projectileSpeed, ctx.dir_y * ctx.weapon._projectileSpeed);
    ctx.r.emplace_component<animation>(projectile, std::string(RTYPE_PATH_ASSETS) + "Binary_bullet-Sheet.png", 220, 220, 0.1f, 0.1f, 0, false);
    ctx.r.emplace_component<lifetime>(projectile);
    ctx.r.emplace_component<Gravity>(projectile, 100.0f);
}

void Shoot::shootHardBullets(const ProjectileContext& ctx)
{
    auto projectile = ctx.r.spawn_entity();

    ctx.r.emplace_component<Projectile>(projectile, Projectile(ctx.owner_entity, ctx.weapon._damage, ctx.weapon._projectileSpeed, ctx.dir_x, ctx.dir_y, 5.0f, 4.0f, true));
    ctx.r.emplace_component<position>(projectile, ctx.spawn_x, ctx.spawn_y);
    ctx.r.emplace_component<velocity>(projectile, ctx.dir_x * ctx.weapon._projectileSpeed, ctx.dir_y * ctx.weapon._projectileSpeed);
    ctx.r.emplace_component<animation>(projectile, std::string(RTYPE_PATH_ASSETS) + "Binary_bullet-Sheet.png", 220, 220, 0.1f, 0.1f, 0, false);
    ctx.r.emplace_component<lifetime>(projectile);
}

void Shoot::shootBigBullets(const ProjectileContext& ctx)
{
    auto projectile = ctx.r.spawn_entity();

    ctx.r.emplace_component<Projectile>(projectile, Projectile(ctx.owner_entity, ctx.weapon._damage * 2, ctx.weapon._projectileSpeed, ctx.dir_x, ctx.dir_y, 5.0f, 110.0f, true));
    ctx.r.emplace_component<position>(projectile, ctx.spawn_x, ctx.spawn_y);
    ctx.r.emplace_component<velocity>(projectile, ctx.dir_x * ctx.weapon._projectileSpeed, ctx.dir_y * ctx.weapon._projectileSpeed);
    ctx.r.emplace_component<animation>(projectile, std::string(RTYPE_PATH_ASSETS) + "Binary_bullet-Sheet.png", 220, 220, 1.0f, 1.0f, 0, false);
    ctx.r.emplace_component<lifetime>(projectile);
}

void Shoot::shootParabolBullets(const ProjectileContext& ctx)
{
    auto projectile = ctx.r.spawn_entity();

    ctx.r.emplace_component<Projectile>(projectile, Projectile(ctx.owner_entity, ctx.weapon._damage, ctx.weapon._projectileSpeed, ctx.dir_x, ctx.dir_y, 5.0f, 4.0f, true));
    ctx.r.emplace_component<position>(projectile, ctx.spawn_x, ctx.spawn_y);
    ctx.r.emplace_component<velocity>(projectile, ctx.dir_x * ctx.weapon._projectileSpeed, ctx.dir_y * ctx.weapon._projectileSpeed);
    ctx.r.emplace_component<animation>(projectile, std::string(RTYPE_PATH_ASSETS) + "pbShoot.gif", 34, 34, 1.0f, 1.0f, 3, false);
    ctx.r.emplace_component<lifetime>(projectile);
    ctx.r.emplace_component<Gravity>(projectile, 500.0f);
    ctx.r.emplace_component<Parabol>(projectile, ctx.spawn_x, ctx.spawn_y, 200.0f, false);

    auto projectile2 = ctx.r.spawn_entity();

    ctx.r.emplace_component<Projectile>(projectile2, Projectile(ctx.owner_entity, ctx.weapon._damage, ctx.weapon._projectileSpeed, ctx.dir_x, ctx.dir_y, 5.0f, 4.0f, true));
    ctx.r.emplace_component<position>(projectile2, ctx.spawn_x, ctx.spawn_y);
    ctx.r.emplace_component<velocity>(projectile2, ctx.dir_x * ctx.weapon._projectileSpeed, ctx.dir_y * ctx.weapon._projectileSpeed);
    ctx.r.emplace_component<animation>(projectile2, std::string(RTYPE_PATH_ASSETS) + "pbShoot.gif", 34, 34, 1.0f, 1.0f, 3, false);
    ctx.r.emplace_component<lifetime>(projectile2);
    ctx.r.emplace_component<Gravity>(projectile2, -500.0f);
    ctx.r.emplace_component<Parabol>(projectile2, ctx.spawn_x, ctx.spawn_y, 200.0f, true);
}

void Shoot::renderHitboxes(registry &r)
{
    auto *projArr = r.get_if<Projectile>();
    auto *posArr = r.get_if<position>();

    if (!projArr || !posArr) return;

    for (auto [proj, pos, projEntity] : zipper(*projArr, *posArr)) {
        float pcenterX = pos.x;
        float pcenterY = pos.y;

        RenderManager::instance().draw_circle(
            static_cast<int>(pcenterX),
            static_cast<int>(pcenterY),
            proj._radius,
            RED
        );
    }
}

void Shoot::update(registry& r, float dt) {
    checkShootIntention(r);
    spawnProjectiles(r, dt);
    checkEnnemyHits(r);
    // renderHitboxes(r);
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<Shoot>();
    }
}
