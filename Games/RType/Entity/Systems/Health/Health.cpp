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


void HealthSys::update(registry& r, float dt ATTR_MAYBE_UNUSED) {
    checkAndKillEnemy(r);
    checkAndKillPlayer(r);
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