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

void HealthSys::update(registry& r, float dt) {
    checkAndKill(r);
}

void HealthSys::checkAndKill(registry &r)
{
    auto *healthArr = r.get_if<Health>();
    auto *enemyArr = r.get_if<enemy>();
    std::vector<entity> entToKill;

    if (!healthArr) return;

    // If we have enemy components, iterate only entities that are both Health + enemy
    if (enemyArr) {
        for (auto [healthEnt, enemyComp, ent] : zipper(*healthArr, *enemyArr)) {
            if (healthEnt._health <= 0) {
                entToKill.push_back(entity(ent));
            }
        }
    } else {
        // fallback: no enemy component present, collect dead health entities anyway
        for (auto [healthEnt, ent] : zipper(*healthArr)) {
            if (healthEnt._health <= 0) {
                entToKill.push_back(entity(ent));
            }
        }
    }

    if (!entToKill.empty()) {
        std::sort(entToKill.begin(), entToKill.end());
        entToKill.erase(std::unique(entToKill.begin(), entToKill.end()), entToKill.end());

        for (auto ent : entToKill) {
            // if we iterated via enemyArr (above), these are enemies -> increment score
            if (enemyArr) {
                addScore(r);
            }
            std::cout << "ennemy killed" << std::endl;
            r.kill_entity(ent);
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
    if (scoreArr && static_cast<size_t>(scoreEnt) < scoreArr->size()) {
        (*scoreArr)[static_cast<size_t>(scoreEnt)]._score += 1;
        std::cout << "score += 1" << std::endl;
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<HealthSys>();
    }
}
