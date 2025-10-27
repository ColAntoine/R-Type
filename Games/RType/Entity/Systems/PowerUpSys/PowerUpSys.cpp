/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PowerUpSys system implementation
*/

#include <memory>
#include <random>
#include <algorithm>
#include <array>
#include <iostream>

#include "PowerUpSys.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/Zipper.hpp"
#include "Entity/Components/Weapon/Weapon.hpp"
#include "ECS/Components/Velocity.hpp"

PowerUpSys::PowerUpSys()
    : rng_(std::random_device{}()),
      x_dist_(0.0f, 800.0f),
      powerup_type_dist_(0, 3)
{

}

void PowerUpSys::update(registry& r, float dt)
{
    spawnPowerUps(r, dt);
    colisionPowerUps(r, dt);
}

void PowerUpSys::spawnPowerUps(registry &r, float dt)
{
    spawn_timer_ += dt;

    if (spawn_timer_ >= spawn_interval_) {
        auto& renderManager = RenderManager::instance();
        float screen_width = renderManager.get_screen_infos().getWidth();
        float screen_height = renderManager.get_screen_infos().getHeight();

        // Update distributions based on screen size
        x_dist_ = std::uniform_real_distribution<>(0.0f, screen_width);
        y_dist_ = std::uniform_real_distribution<>(-50.0f, 0.0f);

        float spawn_x = x_dist_(rng_);
        float spawn_y = y_dist_(rng_);
        float fall_speed = 50.0f + (rng_() % 100);
        powerUpType type = static_cast<powerUpType>(powerup_type_dist_(rng_));

        auto ent = r.spawn_entity();
        r.emplace_component<PowerUp>(ent, type, renderManager.scaleSizeW(5.0f), renderManager.scaleSizeH(5.0f), -renderManager.scaleSizeW(2.5f), -renderManager.scaleSizeH(2.5f));
        r.emplace_component<position>(ent, spawn_x, spawn_y);
        r.emplace_component<collider>(ent, renderManager.scaleSizeW(5.0f), renderManager.scaleSizeH(5.0f), -renderManager.scaleSizeW(2.5f), -renderManager.scaleSizeH(2.5f));
        r.emplace_component<velocity>(ent, 0.0f, fall_speed);
        r.emplace_component<drawable>(ent, renderManager.scaleSizeW(5.0f), renderManager.scaleSizeH(5.0f));

        spawn_timer_ = 0.0f;
    }
}

void PowerUpSys::colisionPowerUps(registry &r, float dt)
{
    auto pUpArr = r.get_if<PowerUp>();
    auto posArr = r.get_if<position>();
    auto colArr = r.get_if<collider>();
    auto playerArr = r.get_if<Player>();
    auto weaponArr = r.get_if<Weapon>();
    auto velArr = r.get_if<velocity>();

    if (!pUpArr || !posArr || !colArr || !playerArr || !weaponArr) return;

    std::vector<size_t> entitiesToKill;

    for (auto [pUp, pPos, pCol, pEntity] : zipper(*pUpArr, *posArr, *colArr)) {
        // Get powerup AABB
        float p_left = pPos.x + pCol.offset_x;
        float p_right = pPos.x + pCol.offset_x + pCol.w;
        float p_top = pPos.y + pCol.offset_y;
        float p_bottom = pPos.y + pCol.offset_y + pCol.h;

        for (auto [player, playerPos, playerCol, playerWeapon, playerVel, playerEntity] : zipper(*playerArr, *posArr, *colArr, *weaponArr, *velArr)) {
            if (pEntity == playerEntity) continue;

            float pl_left = playerPos.x + playerCol.offset_x;
            float pl_right = playerPos.x + playerCol.offset_x + playerCol.w;
            float pl_top = playerPos.y + playerCol.offset_y;
            float pl_bottom = playerPos.y + playerCol.offset_y + playerCol.h;

            bool overlap = !(p_right < pl_left || p_left > pl_right || p_bottom < pl_top || p_top > pl_bottom);

            if (overlap) {
                applyPowerUps(playerWeapon, &playerVel, pUp);
                entitiesToKill.push_back(static_cast<size_t>(pEntity));
                break;
            }
        }
    }

    if (!entitiesToKill.empty()) {
        std::sort(entitiesToKill.begin(), entitiesToKill.end());
        entitiesToKill.erase(std::unique(entitiesToKill.begin(), entitiesToKill.end()), entitiesToKill.end());

        for (auto ent_id : entitiesToKill) {
            if (posArr && posArr->has(ent_id)) {
                r.kill_entity(entity(ent_id));
            }
        }
    }
}

void PowerUpSys::applyPowerUps(Weapon &weapon, velocity *vel, PowerUp &pUp)
{
    switch (pUp._pwType) {
        case PLAYER_SPEED:
            if (vel) {
                vel->vx *= 1.5f;
                vel->vy *= 1.5f;
            }
            std::cout << "Applied PLAYER_SPEED powerup\n";
            break;
        case WEAPON_FIRERATE:
            weapon._fireRate *= 1.5f;
            std::cout << "Applied WEAPON_FIRERATE powerup\n";
            break;
        case WAEPON_COOLDOWN:
            weapon._cooldown *= 0.5f;
            std::cout << "Applied WAEPON_COOLDOWN powerup\n";
            break;
        case WEAPON_NEW:
            {
                std::array<std::string, 4> bulletTypes = {"hardBullet", "bullet", "bigBullet", "parabol"};
                std::uniform_int_distribution<> bullet_dist(0, bulletTypes.size());
                std::string selectedBullet = bulletTypes[bullet_dist(rng_)];

                auto it = std::find(weapon._projectileType.begin(), weapon._projectileType.end(), selectedBullet);
                if (it == weapon._projectileType.end()) {
                    weapon._projectileType.push_back(selectedBullet);
                    std::cout << "Applied WEAPON_NEW powerup: " << selectedBullet << "\n";
                } else {
                    std::cout << "WEAPON_NEW powerup already has: " << selectedBullet << "\n";
                }
            }
            break;
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<PowerUpSys>();
    }
}
