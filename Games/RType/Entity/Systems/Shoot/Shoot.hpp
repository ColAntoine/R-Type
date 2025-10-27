/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Shoot system
*/

#pragma once

#include "Entity/Components/Weapon/Weapon.hpp"
#include "Entity/Components/LifeTime/LifeTime.hpp"
#include "Entity/Components/Controllable/Controllable.hpp"
#include "Entity/Components/Projectile/Projectile.hpp"
#include "Entity/Components/Drawable/Drawable.hpp"
#include "Entity/Components/Health/Health.hpp"
#include "Entity/Components/Enemy/Enemy.hpp"
#include "Entity/Components/Gravity/Gravity.hpp"
#include "Entity/Components/Parabol/Parabol.hpp"

#include "ECS/Systems/ISystem.hpp"

#include "ECS/Registry.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Components.hpp"

#include "Constants.hpp"
#include <map>
#include <functional>

struct ProjectileContext {
    registry& r;
    entity owner_entity;
    const Weapon& weapon;
    float spawn_x;
    float spawn_y;
    float dir_x;
    float dir_y;
};

class Shoot : public ISystem {
public:
    Shoot();
    ~Shoot() override = default;
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "Shoot"; }

private:
    void spawnProjectiles(registry &r, float dt);
    void checkShootIntention(registry & r);
    void checkEnnemyHits(registry & r);
    void renderHitboxes(registry &r);

    /* SHOOT FUNCTIONS */
    void shootBaseBullets(const ProjectileContext& ctx);
    void shootHardBullets(const ProjectileContext& ctx);
    void shootBigBullets(const ProjectileContext& ctx);
    void shootParabolBullets(const ProjectileContext& ctx);
    void shootEnemyBullets(const ProjectileContext& ctx);

    std::map<std::string, std::function<void(const ProjectileContext&)>> _shootType;
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
