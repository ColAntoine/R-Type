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
#include "Entity/Components/Health/Health.hpp"
#include "ECS/Messaging/MessagingManager.hpp"
#include "Constants.hpp"

PowerUpSys::PowerUpSys()
: _rng(std::random_device{}()),
    _x_dist(0.0f, 800.0f),
    _powerup_type_dist(0, 4)
{
    _pUpText[WEAPON_FIRERATE] = "Firerate increased!";
    _pUpText[PLAYER_SPEED] = "Player speed increased!";
    _pUpText[WEAPON_NEW] = "New weapon unlocked";
    _pUpText[HEALTH_UP] = "Health increased!";
    _pUpText[WEAPON_DAMAGE] = "Weapon damage increased!";
}

void PowerUpSys::update(registry& r, float dt)
{
    spawnPowerUps(r, dt);
    colisionPowerUps(r, dt);
}

void PowerUpSys::spawnPowerUps(registry &r, float dt)
{
    _spawn_timer += dt;

    if (_spawn_timer >= _spawn_interval) {
        auto& renderManager = RenderManager::instance();
        float screen_width = renderManager.get_screen_infos().getWidth();
        float screen_height = renderManager.get_screen_infos().getHeight();

        float left_third = screen_width / 3.0f;
        _x_dist = std::uniform_real_distribution<>(0.0f, left_third);
        _y_dist = std::uniform_real_distribution<>(-50.0f, 0.0f);

        float spawn_x = _x_dist(_rng);
        float spawn_y = _y_dist(_rng);
        float fall_speed = 50.0f + (_rng() % 100);
        powerUpType type = static_cast<powerUpType>(_powerup_type_dist(_rng));

        auto ent = r.spawn_entity();
        float pup_width = GET_SCALE_X(5.0f, screen_width);
        float pup_height = GET_SCALE_Y(5.0f, screen_height);
        float pup_offset_x = -pup_width / 2.0f;
        float pup_offset_y = -pup_height / 2.0f;

        r.emplace_component<PowerUp>(ent, type, pup_width, pup_height, pup_offset_x, pup_offset_y);
        r.emplace_component<position>(ent, spawn_x, spawn_y);
        r.emplace_component<collider>(ent, pup_width, pup_height, pup_offset_x, pup_offset_y);
        r.emplace_component<velocity>(ent, 0.0f, fall_speed);

        float target_w = screen_width * 0.03f;
        float target_h = screen_height * 0.03f;

        switch (type)
        {
        case WEAPON_FIRERATE: {
            float orig_w = 512.f, orig_h = 512.f;
            float sx = GET_SCALE_X(target_w / orig_w, screen_width);
            float sy = GET_SCALE_Y(target_h / orig_h, screen_height);
            r.emplace_component<sprite>(ent, std::string(RTYPE_PATH_ASSETS) + "PowerUps/firerate.png", orig_w, orig_h, sx, sy);
            break;
        }
        case WEAPON_NEW: {
            float orig_w = 315.f, orig_h = 250.f;
            float sx = GET_SCALE_X(target_w / orig_w, screen_width);
            float sy = GET_SCALE_Y(target_h / orig_h, screen_height);
            r.emplace_component<sprite>(ent, std::string(RTYPE_PATH_ASSETS) + "PowerUps/gun.png", orig_w, orig_h, sx, sy);
            break;
        }
        case PLAYER_SPEED: {
            float orig_w = 512.f, orig_h = 512.f;
            float sx = GET_SCALE_X(target_w / orig_w, screen_width);
            float sy = GET_SCALE_Y(target_h / orig_h, screen_height);
            r.emplace_component<sprite>(ent, std::string(RTYPE_PATH_ASSETS) + "PowerUps/speed.png", orig_w, orig_h, sx, sy);
            break;
        }
        case HEALTH_UP: {
            float orig_w = 254.f, orig_h = 254.f;
            float sx = GET_SCALE_X(target_w / orig_w, screen_width);
            float sy = GET_SCALE_Y(target_h / orig_h, screen_height);
            r.emplace_component<sprite>(ent, std::string(RTYPE_PATH_ASSETS) + "PowerUps/health.png", orig_w, orig_h, sx, sy);
            break;
        }
        case WEAPON_DAMAGE: {
            float orig_w = 1024.f, orig_h = 1024.f;
            float sx = GET_SCALE_X(target_w / orig_w, screen_width);
            float sy = GET_SCALE_Y(target_h / orig_h, screen_height);
            r.emplace_component<sprite>(ent, std::string(RTYPE_PATH_ASSETS) + "PowerUps/damage.png", orig_w, orig_h, sx, sy);
            break;
        }
        }

        _spawn_timer = 0.0f;
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
    auto healthArr = r.get_if<Health>();
    auto ctrlArr = r.get_if<controllable>();

    if (!pUpArr || !posArr || !colArr || !playerArr || !weaponArr || !velArr || !healthArr || !ctrlArr) return;

    std::vector<size_t> entitiesToKill;

    for (auto [pUp, pPos, pCol, pEntity] : zipper(*pUpArr, *posArr, *colArr)) {
        float p_left = pPos.x + pCol.offset_x;
        float p_right = pPos.x + pCol.offset_x + pCol.w;
        float p_top = pPos.y + pCol.offset_y;
        float p_bottom = pPos.y + pCol.offset_y + pCol.h;

        for (auto [player, playerPos, playerCol, playerWeapon, playerVel, playerHealth, playerCtrl, playerEntity] : zipper(*playerArr, *posArr, *colArr, *weaponArr, *velArr, *healthArr, *ctrlArr)) {
            if (pEntity == playerEntity) continue;

            float pl_left = playerPos.x + playerCol.offset_x;
            float pl_right = playerPos.x + playerCol.offset_x + playerCol.w;
            float pl_top = playerPos.y + playerCol.offset_y;
            float pl_bottom = playerPos.y + playerCol.offset_y + playerCol.h;

            bool overlap = !(p_right < pl_left || p_left > pl_right || p_bottom < pl_top || p_top > pl_bottom);

            if (overlap) {
                int wave = getWave(r);
                applyPowerUps(playerWeapon, &playerVel, &playerHealth, &playerCtrl, pUp, wave);
                entitiesToKill.push_back(static_cast<size_t>(pEntity));
                auto animEnt = r.spawn_entity();
                if (_pUpText.find(pUp._pwType) != _pUpText.end()) {
                    r.emplace_component<PUpAnimation>(animEnt, true, _pUpText[pUp._pwType]);
                }
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

void PowerUpSys::applyPowerUps(Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, PowerUp &pUp, int wave)
{
    switch (pUp._pwType) {
        case PLAYER_SPEED:
            if (vel) {
                vel->vx *= 1.5f;
                vel->vy *= 1.5f;
            }
            if (ctrl) {
                ctrl->speed *= 1.5f;
            }
            std::cout << "Applied PLAYER_SPEED powerup\n";
            {
                Event statsEvent("PLAYER_STATS_CHANGED");
                statsEvent.set("speed", static_cast<int>(ctrl->speed));
                statsEvent.set("firerate", static_cast<int>(weapon._fireRate * 100));
                statsEvent.set("damage", static_cast<int>(weapon._damage));
                MessagingManager::instance().get_event_bus().emit(statsEvent);
            }
            break;
        case WEAPON_FIRERATE:
            weapon._fireRate *= 1.5f;
            std::cout << "Applied WEAPON_FIRERATE powerup\n";
            {
                Event statsEvent("PLAYER_STATS_CHANGED");
                statsEvent.set("speed", static_cast<int>(ctrl->speed));
                statsEvent.set("firerate", static_cast<int>(weapon._fireRate * 100));
                statsEvent.set("damage", static_cast<int>(weapon._damage));
                MessagingManager::instance().get_event_bus().emit(statsEvent);
            }
            break;
        case WEAPON_NEW:
            {
                std::array<std::string, 4> bulletTypes = {"hardBullet", "bullet", "bigBullet", "parabol"};
                std::uniform_int_distribution<> bullet_dist(0, bulletTypes.size() - 1);
                std::string selectedBullet = bulletTypes[bullet_dist(_rng)];

                auto it = std::find(weapon._projectileType.begin(), weapon._projectileType.end(), selectedBullet);
                if (it == weapon._projectileType.end()) {
                    weapon._projectileType.push_back(selectedBullet);
                    std::cout << "Applied WEAPON_NEW powerup: " << selectedBullet << "\n";
                } else {
                    std::cout << "WEAPON_NEW powerup already has: " << selectedBullet << "\n";
                }
            }
            break;
        case HEALTH_UP:
            if (health) {
                int increase = 20 * (wave + 1);
                health->_health += increase;
                std::cout << "Applied HEALTH_UP powerup: +" << increase << " health\n";
                Event healthEvent("PLAYER_HEALTH_CHANGED");
                healthEvent.set("health", static_cast<int>(health->_health));
                MessagingManager::instance().get_event_bus().emit(healthEvent);
            }
            break;
        case WEAPON_DAMAGE:
            {
                int increase = 5 * (wave + 1);
                weapon._damage += increase;
                std::cout << "Applied WEAPON_DAMAGE powerup: +" << increase << " damage\n";
                Event statsEvent("PLAYER_STATS_CHANGED");
                statsEvent.set("speed", static_cast<int>(ctrl->speed));
                statsEvent.set("firerate", static_cast<int>(weapon._fireRate * 100));
                statsEvent.set("damage", static_cast<int>(weapon._damage));
                MessagingManager::instance().get_event_bus().emit(statsEvent);
            }
            break;
    }
}

int PowerUpSys::getWave(registry &r)
{
    auto waveArr = r.get_if<CurrentWave>();

    if (!waveArr) return 0;

    for (auto [wave, ent]: zipper(*waveArr)) {
        return wave._currentWave;
    }
    return 0;
}

DLL_EXPORT ISystem* create_system() {
    try {
        return new PowerUpSys();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}