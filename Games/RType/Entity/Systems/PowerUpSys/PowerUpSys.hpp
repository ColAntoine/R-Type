/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PowerUpSys system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"
#include "ECS/Registry.hpp"

#include "ECS/Components/Collider.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/Components/Sprite.hpp"

#include "Entity/Components/PowerUp/PowerUp.hpp"
#include "Entity/Components/Drawable/Drawable.hpp"
#include "Entity/Components/Weapon/Weapon.hpp"
#include "Entity/Components/Health/Health.hpp"
#include "Entity/Components/CurrentWave/CurrentWave.hpp"
#include "Entity/Components/Player/Player.hpp"
#include "Entity/Components/PUpAnimation/PUpAnimation.hpp"
#include "Entity/Components/Controllable/Controllable.hpp"

#include "Constants.hpp"

#include <map>
#include <functional>
#include <random>

class PowerUpSys : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "PowerUpSys"; }

    PowerUpSys();
    void seed_from_registry(registry& r);

private:
    void spawnPowerUps(registry &r, float dt);
    void colisionPowerUps(registry &r, float dt);
    void applyPowerUps(Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, PowerUp &pUp, int wave);
    int getWave(registry &r);
    void initPowerUpHandlers();

    // Power-up handler methods
    void handlePlayerSpeed(Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, int wave);
    void handleWeaponFirerate(Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, int wave);
    void handleWeaponNew(Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, int wave);
    void handleHealthUp(Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, int wave);
    void handleWeaponDamage(Weapon &weapon, velocity *vel, Health *health, controllable *ctrl, int wave);

    // Sprite emplacement utility
    void emplacePowerUpSprite(registry &r, entity ent, powerUpType type, const std::string &texturePath,
        float origWidth, float origHeight, float screenWidth, float screenHeight);

    std::map<powerUpType, std::function<void(registry &)>> _pFunc;
    std::map<powerUpType, std::string> _pUpText;
    std::map<powerUpType, std::function<void(Weapon&, velocity*, Health*, controllable*, int)>> _powerUpHandlers;

    std::mt19937 _rng;
    std::uniform_real_distribution<> _x_dist;
    std::uniform_real_distribution<> _y_dist;
    std::uniform_int_distribution<> _powerup_type_dist;
    float _spawn_timer{0.0f};
    float _spawn_interval{30.0f};
    bool _seeded{false};
};


#if defined(_WIN32)
  #define DLL_EXPORT extern "C" __declspec(dllexport)
#else
  #define DLL_EXPORT extern "C"
#endif

DLL_EXPORT ISystem* create_system();
DLL_EXPORT void     destroy_system(ISystem* ptr);
