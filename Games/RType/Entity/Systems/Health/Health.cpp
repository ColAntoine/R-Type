/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Health system implementation
*/

#include <memory>
#include <algorithm>
#include <iostream>

#include "Health.hpp"

#include "Entity/Components/Score/Score.hpp"
#include "Entity/Components/Enemy/Enemy.hpp"
#include "Entity/Components/Weapon/Weapon.hpp"
#include "Entity/Components/Controllable/Controllable.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Messaging/MessagingManager.hpp"
#include "ECS/Renderer/RenderManager.hpp"

#include "Constants.hpp"

#if defined(_MSC_VER)
  #define ATTR_MAYBE_UNUSED [[maybe_unused]]
#else
  #define ATTR_MAYBE_UNUSED __attribute__((unused))
#endif

// Global callback for entity destruction broadcasting
static EntityDestroyCallback g_entity_destroy_callback = nullptr;

void set_global_entity_destroy_callback(EntityDestroyCallback callback) {
    g_entity_destroy_callback = std::move(callback);
}


void HealthSys::update(registry& r, float dt ATTR_MAYBE_UNUSED) {
    checkAndKillEnemy(r);
    checkAndKillPlayer(r);
    emitPlayerHealthStats(r);
}

void HealthSys::checkAndKillEnemy(registry &r)
{
    auto *healthArr = r.get_if<Health>();
    auto *posArr = r.get_if<position>();
    auto *enemyArr = r.get_if<Enemy>();
    auto *colArr = r.get_if<collider>();
    std::vector<entity> entToKill;

    if (!healthArr || !enemyArr || !colArr) return;

    for (auto [healthEnt, enemyComp, pos, col, ent] : zipper(*healthArr, *enemyArr, *posArr, *colArr)) {
        if (healthEnt._health <= 0) {
            entToKill.push_back(entity(ent));

            entity anim = r.spawn_entity();
            float frame_w = DEATH_ANIM_FRAME_W;
            float frame_h = DEATH_ANIM_FRAME_H;
            float screen_w = RenderManager::instance().get_screen_infos().getWidth();
            float scale = GET_SCALE_X(col.w / frame_w, screen_w);

            r.emplace_component<animation>(anim, RTYPE_PATH_ASSETS + "EnemyDeath.png", frame_w, frame_h, scale, scale, 10, false, true);
            r.emplace_component<position>(anim, pos.x, pos.y);
        }
    }

    if (!entToKill.empty()) {
        std::sort(entToKill.begin(), entToKill.end());
        entToKill.erase(std::unique(entToKill.begin(), entToKill.end()), entToKill.end());

        for (auto ent : entToKill) {
            if (r.get_if<Health>() && r.get_if<Health>()->has(static_cast<size_t>(ent))) {
                addScore(r);

                std::cout << "[HealthSys] Killing enemy entity=" << ent << std::endl;

                // Notify server to broadcast entity destruction
                if (destroy_callback_) {
                    destroy_callback_(ent, 0); // reason 0 = killed by damage
                } else if (g_entity_destroy_callback) {
                    g_entity_destroy_callback(ent, 0);
                }

                r.kill_entity(ent);
            }
        }
    }
}

void HealthSys::checkAndKillPlayer(registry &r)
{
    auto *healthArr = r.get_if<Health>();
    auto *playerArr = r.get_if<Player>();
    auto *posArr = r.get_if<position>();
    auto *colArr = r.get_if<collider>();
    std::vector<entity> entToKill;

    if (!healthArr || !playerArr || !posArr || !colArr) return;

    for (auto [healthEnt, player, pos, col, ent] : zipper(*healthArr, *playerArr, *posArr, *colArr)) {
        if (healthEnt._health <= 0) {
            entToKill.push_back(entity(ent));
            entity anim = r.spawn_entity();
            float frame_w = DEATH_ANIM_FRAME_W;
            float frame_h = DEATH_ANIM_FRAME_H;
            float screen_w = RenderManager::instance().get_screen_infos().getWidth();
            float scale = GET_SCALE_X(col.w / frame_w, screen_w);

            r.emplace_component<animation>(anim, RTYPE_PATH_ASSETS + "EnemyDeath.png", frame_w, frame_h, scale, scale, 10, false, true);
            r.emplace_component<position>(anim, pos.x, pos.y);
        }
    }

    if (!entToKill.empty()) {
        std::sort(entToKill.begin(), entToKill.end());
        entToKill.erase(std::unique(entToKill.begin(), entToKill.end()), entToKill.end());

        for (auto ent : entToKill) {
            // Check if entity still exists before killing
            if (r.get_if<Health>() && r.get_if<Health>()->has(static_cast<size_t>(ent))) {
                r.kill_entity(ent);
            }
        }
    }
}

void HealthSys::addScore(registry &r, int amount)
{
    // find existing Score entity or create one
    auto *scoreArr = r.get_if<Score>();
    entity scoreEnt = static_cast<entity>(-1);

    if (scoreArr) {
        for (auto [s, ent] : zipper(*scoreArr)) {
            scoreEnt = entity(ent);
            break;
        }
    }

    if (scoreEnt == static_cast<entity>(-1)) {
        scoreEnt = r.spawn_entity();
        r.emplace_component<Score>(scoreEnt, Score(0));
    }

    // increment the score by 1
    scoreArr = r.get_if<Score>();
    if (scoreArr && scoreArr->has(static_cast<size_t>(scoreEnt))) {
        scoreArr->get(static_cast<size_t>(scoreEnt))._score += amount;
        Event scoreEvent("SCORE_INCREASED");
        scoreEvent.set("amount", amount);
        MessagingManager::instance().get_event_bus().emit(scoreEvent);
    }
}

void HealthSys::emitPlayerHealthStats(registry &r)
{
    auto *playerArr = r.get_if<Player>();
    auto *healthArr = r.get_if<Health>();
    auto *weaponArr = r.get_if<Weapon>();
    auto *ctrlArr = r.get_if<controllable>();

    if (!playerArr || !healthArr || !weaponArr || !ctrlArr) return;

    for (auto [player, health, weapon, ctrl, ent] : zipper(*playerArr, *healthArr, *weaponArr, *ctrlArr)) {
        // Emit health change
        Event healthEvent("PLAYER_HEALTH_CHANGED");
        healthEvent.set("health", static_cast<int>(health._health));
        MessagingManager::instance().get_event_bus().emit(healthEvent);

        // Emit stats change
        Event statsEvent("PLAYER_STATS_CHANGED");
        statsEvent.set("speed", static_cast<int>(ctrl.speed));
        statsEvent.set("firerate", static_cast<int>(weapon._fireRate * 100));
        statsEvent.set("damage", static_cast<int>(weapon._damage));
        MessagingManager::instance().get_event_bus().emit(statsEvent);
        return;
    }
}

DLL_EXPORT ISystem* create_system() {
    try {
        return new HealthSys();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}