/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Health system implementation
*/

#include <memory>
#include <algorithm>

#include "Health.hpp"

void HealthSys::update(registry& r, float dt) {
    checkAndKill(r);
}

void HealthSys::checkAndKill(registry &r)
{
    auto *healthArr = r.get_if<Health>();
    std::vector<entity> entToKill;

    if (!healthArr) return;

    for (auto [healthEnt, healthEntitiy] : zipper(*healthArr)) {
        if (healthEnt._health <= 0) {
            entToKill.push_back(entity(healthEntitiy));
        }
    }

    if (!entToKill.empty()) {
        std::sort(entToKill.begin(), entToKill.end());
        entToKill.erase(std::unique(entToKill.begin(), entToKill.end()), entToKill.end());

        for (auto ent : entToKill) {
            r.kill_entity(ent);
        }
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<HealthSys>();
    }
}
