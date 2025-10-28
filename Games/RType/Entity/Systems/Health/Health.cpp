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

void HealthSys::update(registry& r, float dt __attribute_maybe_unused__) {
    checkAndKillEnemy(r);
    checkAndKillPlayer(r);
}

void HealthSys::checkAndKillEnemy(registry &r)
{
    auto *healthArr = r.get_if<Health>();
    auto *enemyArr = r.get_if<Enemy>();
    std::vector<entity> entToKill;

    if (!healthArr || !enemyArr) return;

    for (auto [healthEnt, enemyComp, ent] : zipper(*healthArr, *enemyArr)) {
        if (healthEnt._health <= 0) {
            entToKill.push_back(entity(ent));
        }
    }

    if (!entToKill.empty()) {
        std::sort(entToKill.begin(), entToKill.end());
        entToKill.erase(std::unique(entToKill.begin(), entToKill.end()), entToKill.end());

        for (auto ent : entToKill) {
            if (r.get_if<Health>() && r.get_if<Health>()->has(static_cast<size_t>(ent))) {
                addScore(r);
                // add a death animation component here
                r.kill_entity(ent);
            }
        }
    }
}

void HealthSys::checkAndKillPlayer(registry &r)
{
    auto *healthArr = r.get_if<Health>();
    std::vector<entity> entToKill;

    if (!healthArr) return;

    for (auto [healthEnt, ent] : zipper(*healthArr)) {
        if (healthEnt._health <= 0) {
            entToKill.push_back(entity(ent));
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
    // // find existing Score entity or create one
    // auto *scoreArr = r.get_if<Score>();
    // entity scoreEnt = static_cast<entity>(-1);

    // if (scoreArr) {
    //     for (auto [s, ent] : zipper(*scoreArr)) {
    //         scoreEnt = entity(ent);
    //         break;
    //     }
    // }

    // if (scoreEnt == static_cast<entity>(-1)) {
    //     scoreEnt = r.spawn_entity();
    //     r.emplace_component<Score>(scoreEnt, Score(0));
    // }

    // // increment the score by 1
    // scoreArr = r.get_if<Score>();
    // if (scoreArr && scoreArr->has(static_cast<size_t>(scoreEnt))) {
    //     scoreArr->get(static_cast<size_t>(scoreEnt))._score += 1;
    // }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<HealthSys>();
    }
}
