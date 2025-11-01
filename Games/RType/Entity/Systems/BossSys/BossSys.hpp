/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** BossSys system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/Components/Animation.hpp"
#include "ECS/Components/Collider.hpp"

#include "Entity/Components/Weapon/Weapon.hpp"
#include "Entity/Components/Health/Health.hpp"
#include "Entity/Components/Boss/Boss.hpp"
#include "Entity/Components/Enemy/Enemy.hpp"
#include "Entity/Components/Player/Player.hpp"
#include "Entity/Components/CurrentWave/CurrentWave.hpp"

// TODO: the draw is temporary until we find some sprites
#include "Entity/Components/Drawable/Drawable.hpp"

#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Messaging/MessagingManager.hpp"
#include "ECS/Audio/AudioManager.hpp"

#include "Constants.hpp"

#include <map>
#include <vector>

/**
 *  To make the boss work you will need those components:
 *      - pos
 *      - Health
 *      - velocity
 *      - Boss
 *      - Weapon
 *      - Enemy
 *      - collider
 * ?    - animation
*/

namespace EventTypes {
    const std::string PLAYER_CLOSE = "PLAYER_CLOSE";
}

class BossSys : public ISystem {
public:
    BossSys();
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "BossSys"; }

private:
    /* Spawn detection function (will be implemented based on the score or the dt) */
    bool shouldSpawn(registry &r, float dt);
    void spawn(registry &r);

    /* spawn until position 3/4 of the screen */
    void move(registry &r);

    /* Boss Pattern functions */
    // ? punch funcs
    bool isPlayerClose(registry &r);
    int getWave(registry &r);
    void increaseWave(registry &r);
    void startBossMusic(registry &r);
    void stopBossMusic(registry &r);

    // * weapons depending on wave for now
    // * the map use the wave as index and a vector of string to set the weapons
    std::map<int, std::vector<std::string>> _bossWeapons;

    // * RenderManager for quick access
    RenderManager &_renderManager;
};

#if defined(_WIN32)
  #define DLL_EXPORT extern "C" __declspec(dllexport)
#else
  #define DLL_EXPORT extern "C"
#endif

DLL_EXPORT ISystem* create_system();
DLL_EXPORT void     destroy_system(ISystem* ptr);