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
#include "Entity/Components/RemotePlayer/RemotePlayer.hpp"
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

    initPowerUpHandlers();
}

void PowerUpSys::update(registry& r, float dt)
{
    // Seed RNG from registry once if available (server provides game seed in multiplayer)
    if (!_seeded) {
        seed_from_registry(r);
        _seeded = true;
    }

    spawnPowerUps(r, dt);
    colisionPowerUps(r, dt);
}

void PowerUpSys::seed_from_registry(registry& r) {
    if (r.has_random_seed()) {
        _rng.seed(r.get_random_seed());
        std::cout << "[PowerUpSys] Seeded RNG with: " << r.get_random_seed() << std::endl;
    } else {
        // fall back to random_device but keep the default behavior
        _rng.seed(std::random_device{}());
        std::cout << "[PowerUpSys] No registry seed found, using random seed" << std::endl;
    }
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
        float pup_width = GET_SCALE_X(30.0f, screen_width);
        float pup_height = GET_SCALE_Y(30.0f, screen_height);
        float pup_offset_x = -pup_width / 2.0f;
        float pup_offset_y = -pup_height / 2.0f;

        r.emplace_component<PowerUp>(ent, type, pup_width, pup_height, pup_offset_x, pup_offset_y);
        r.emplace_component<position>(ent, spawn_x, spawn_y);
        r.emplace_component<collider>(ent, pup_width, pup_height, pup_offset_x, pup_offset_y);
        r.emplace_component<velocity>(ent, 0.0f, fall_speed);

        switch (type)
        {
        case WEAPON_FIRERATE:
            emplacePowerUpSprite(r, ent, type, std::string(RTYPE_PATH_ASSETS) + "PowerUps/firerate.png", 512.f, 512.f, screen_width, screen_height);
            break;
        case WEAPON_NEW:
            emplacePowerUpSprite(r, ent, type, std::string(RTYPE_PATH_ASSETS) + "PowerUps/gun.png", 315.f, 250.f, screen_width, screen_height);
            break;
        case PLAYER_SPEED:
            emplacePowerUpSprite(r, ent, type, std::string(RTYPE_PATH_ASSETS) + "PowerUps/speed.png", 512.f, 512.f, screen_width, screen_height);
            break;
        case HEALTH_UP:
            emplacePowerUpSprite(r, ent, type, std::string(RTYPE_PATH_ASSETS) + "PowerUps/health.png", 254.f, 254.f, screen_width, screen_height);
            break;
        case WEAPON_DAMAGE:
            emplacePowerUpSprite(r, ent, type, std::string(RTYPE_PATH_ASSETS) + "PowerUps/damage.png", 1024.f, 1024.f, screen_width, screen_height);
            break;
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
    auto remoteArr = r.get_if<remote_player>();
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

        bool powerup_collected = false;

        // Check collision with local Player entities
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
                r.emplace_component<PUpAnimation>(animEnt, true, _pUpText[pUp._pwType]);
                powerup_collected = true;
                break;
            }
        }

        // If not collected yet, check collision with remote_player entities
        if (!powerup_collected && remoteArr) {
            for (auto [remote, playerPos, playerCol, playerWeapon, playerVel, playerHealth, playerCtrl, playerEntity] : zipper(*remoteArr, *posArr, *colArr, *weaponArr, *velArr, *healthArr, *ctrlArr)) {
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
    auto it = _powerUpHandlers.find(pUp._pwType);
    if (it != _powerUpHandlers.end()) {
        it->second(weapon, vel, health, ctrl, wave);
    }
}

void PowerUpSys::initPowerUpHandlers()
{
    _powerUpHandlers[PLAYER_SPEED] = [this](Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, int wave) {
        handlePlayerSpeed(weapon, vel, health, ctrl, wave);
    };

    _powerUpHandlers[WEAPON_FIRERATE] = [this](Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, int wave) {
        handleWeaponFirerate(weapon, vel, health, ctrl, wave);
    };

    _powerUpHandlers[WEAPON_NEW] = [this](Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, int wave) {
        handleWeaponNew(weapon, vel, health, ctrl, wave);
    };

    _powerUpHandlers[HEALTH_UP] = [this](Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, int wave) {
        handleHealthUp(weapon, vel, health, ctrl, wave);
    };

    _powerUpHandlers[WEAPON_DAMAGE] = [this](Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, int wave) {
        handleWeaponDamage(weapon, vel, health, ctrl, wave);
    };
}

void PowerUpSys::handlePlayerSpeed(Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, int wave)
{
    if (vel) {
        vel->vx += 1.17f * static_cast<float>(wave);
        vel->vy += 1.17f * static_cast<float>(wave);
    }
    if (ctrl) {
        ctrl->speed *= 1.17f;
    }
    std::cout << "Applied PLAYER_SPEED powerup\n";
    Event statsEvent("PLAYER_STATS_CHANGED");
    statsEvent.set("speed", static_cast<int>(ctrl->speed));
    statsEvent.set("firerate", static_cast<int>(weapon._fireRate * 100));
    statsEvent.set("damage", static_cast<int>(weapon._damage));
    MessagingManager::instance().get_event_bus().emit(statsEvent);
}

void PowerUpSys::handleWeaponFirerate(Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, int wave)
{
    weapon._fireRate += 1.2f * static_cast<float>(wave + 1);
    std::cout << "Applied WEAPON_FIRERATE powerup\n";
    Event statsEvent("PLAYER_STATS_CHANGED");
    statsEvent.set("speed", static_cast<int>(ctrl->speed));
    statsEvent.set("firerate", static_cast<int>(weapon._fireRate * 100));
    statsEvent.set("damage", static_cast<int>(weapon._damage));
    MessagingManager::instance().get_event_bus().emit(statsEvent);
}

void PowerUpSys::handleWeaponNew(Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, int wave)
{
    std::array<std::string, 4> bulletTypes = {"hardBullet", "bullet", "bigBullet", "parabol"};
    size_t bulletIndex = _rng() % bulletTypes.size();
    std::string selectedBullet = bulletTypes[bulletIndex];

    auto it = std::find(weapon._projectileType.begin(), weapon._projectileType.end(), selectedBullet);
    if (it == weapon._projectileType.end()) {
        weapon._projectileType.push_back(selectedBullet);
        std::cout << "Applied WEAPON_NEW powerup: " << selectedBullet << "\n";
    } else {
        std::cout << "WEAPON_NEW powerup already has: " << selectedBullet << "\n";
    }
}

void PowerUpSys::handleHealthUp(Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, int wave)
{
    if (health) {
        int increase = 30 * (wave + 1);
        health->_health += increase;
        std::cout << "Applied HEALTH_UP powerup: +" << increase << " health\n";
        Event healthEvent("PLAYER_HEALTH_CHANGED");
        healthEvent.set("health", static_cast<int>(health->_health));
        MessagingManager::instance().get_event_bus().emit(healthEvent);
    }
}

void PowerUpSys::handleWeaponDamage(Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, int wave)
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

int PowerUpSys::getWave(registry &r)
{
    auto waveArr = r.get_if<CurrentWave>();

    if (!waveArr) return 0;

    for (auto [wave, ent]: zipper(*waveArr)) {
        return wave._currentWave;
    }
    return 0;
}

void PowerUpSys::emplacePowerUpSprite(registry &r, entity ent, powerUpType type, const std::string &texturePath, 
    float origWidth, float origHeight, float screenWidth, float screenHeight)
{
    float target_w = screenWidth * 0.03f;
    float target_h = screenHeight * 0.03f;
    float sx = GET_SCALE_X(target_w / origWidth, screenWidth);
    float sy = GET_SCALE_Y(target_h / origHeight, screenHeight);
    r.emplace_component<sprite>(ent, texturePath, origWidth, origHeight, sx, sy);
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