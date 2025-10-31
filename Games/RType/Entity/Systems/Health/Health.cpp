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

#include "Constants.hpp"

void HealthSys::update(registry& r, float dt __attribute_maybe_unused__) {
    checkAndKillEnemy(r);
    checkAndKillPlayer(r);
}

void HealthSys::checkAndKillEnemy(registry &r)
{
    auto *healthArr = r.get_if<Health>();
    auto *posArr = r.get_if<position>();
    auto *enemyArr = r.get_if<Enemy>();
    std::vector<entity> entToKill;

    if (!healthArr || !enemyArr) return;

    for (auto [healthEnt, enemyComp, pos, ent] : zipper(*healthArr, *enemyArr, *posArr)) {
        if (healthEnt._health <= 0) {
            entToKill.push_back(entity(ent));

            entity anim = r.spawn_entity();
            float frame_w = 105.0f;
            float frame_h = 107.0f;
            r.emplace_component<animation>(anim, std::string(RTYPE_PATH_ASSETS) + "EnemyDeath.png", frame_w, frame_h, 1.2f, 1.2f, 10, false, true);
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
    std::vector<entity> entToKill;

    if (!healthArr || !playerArr || !posArr) return;

    for (auto [healthEnt, player, pos, ent] : zipper(*healthArr, *playerArr, *posArr)) {
        if (healthEnt._health <= 0) {
            entToKill.push_back(entity(ent));
            entity anim = r.spawn_entity();
            float frame_w = 105.0f;
            float frame_h = 107.0f;
            r.emplace_component<animation>(anim, std::string(RTYPE_PATH_ASSETS) + "EnemyDeath.png", frame_w, frame_h, 1.2f, 1.2f, 10, false, true);
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

void HealthSys::addScore(registry &r)
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
        scoreArr->get(static_cast<size_t>(scoreEnt))._score += 1;
        std::cout << "SCORE: " << scoreArr->get(static_cast<size_t>(scoreEnt))._score << std::endl;
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<HealthSys>();
    }
}
