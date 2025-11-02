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
#include "Entity/Components/Input/Input.hpp"
#include "Entity/Components/Projectile/Projectile.hpp"
#include "Entity/Components/Drawable/Drawable.hpp"
#include "Entity/Components/Health/Health.hpp"
#include "Entity/Components/Enemy/Enemy.hpp"
#include "Entity/Components/Gravity/Gravity.hpp"
#include "Entity/Components/Parabol/Parabol.hpp"
#include "Entity/Components/Following/Following.hpp"
#include "Entity/Components/Wave/Wave.hpp"

#include "ECS/Systems/ISystem.hpp"

#include "ECS/Registry.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Components.hpp"
#include "ECS/Messaging/MessagingManager.hpp"
#include "ECS/Audio/AudioManager.hpp"

#include "Constants.hpp"
#include <map>
#include <functional>

namespace EventTypes {
    const std::string PLAYER_CLOSE = "PLAYER_CLOSE";
}

struct ProjectileContext {
    registry& r;
    entity owner_entity;
    Weapon& weapon;
    float spawn_x;
    float spawn_y;
    float dir_x;
    float dir_y;
    bool _shouldShootSpecial{false};
    float speed_scale{1.0f};
    float frame_width{220.0f};
    float frame_height{220.0f};
};

class Shoot : public ISystem {
public:
    Shoot();
    ~Shoot() override = default;
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "Shoot"; }

private:
    AudioManager& _audioManager = AudioManager::instance();
    float _lastLaserSoundTime = 0.0f;
    const float _laserSoundCooldown = 0.05f;  // 50ms minimum between laser sounds

    void spawnProjectiles(registry &r, float dt);
    void checkShootIntention(registry & r);
    void checkEnnemyHits(registry & r);
    void checkPlayerHits(registry & r);
    void renderHitboxes(registry &r);

    /* SHOOT FUNCTIONS */
    void shootBaseBullets(const ProjectileContext& ctx);
    void shootHardBullets(const ProjectileContext& ctx);
    void shootBigBullets(const ProjectileContext& ctx);
    void shootParabolBullets(const ProjectileContext& ctx);
    void shootEnemyBullets(const ProjectileContext& ctx);

    /* Boss shoots */
    void shootDropBullets(const ProjectileContext& ctx);
    void shootFollowingBullets(const ProjectileContext& ctx);
    void shootWaveBullets(const ProjectileContext& ctx);
    void shootExplosionBullets(const ProjectileContext& ctx);

    std::map<std::string, std::function<void(const ProjectileContext&)>> _shootType;
    EventBus::CallbackId _playerCloseCallBackId;
};


#if defined(_WIN32)
  #define DLL_EXPORT extern "C" __declspec(dllexport)
#else
  #define DLL_EXPORT extern "C"
#endif

DLL_EXPORT ISystem* create_system();
DLL_EXPORT void     destroy_system(ISystem* ptr);
