/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Shoot system implementation
*/

#include <memory>
#include <iostream>

#include "Shoot.hpp"

void Shoot::update(registry& r, float dt) {
    updateCd(r, dt);
    checkShoot(r);
}

void Shoot::checkShoot(registry &r)
{
    auto *playerArr = r.get_if<Player>();
    bool isShooting = IsKeyDown(KEY_SPACE);

    if (!playerArr) return;

    for (auto [player, ent] : zipper(*playerArr)) {
        if (isShooting && player._cooldown <= 0.f) {
            player._isShooting = true;
            player._cooldown = 2.f;
            std::cout << "The player is shooting" << std::endl;
        }
    }
}

void Shoot::updateCd(registry &r, float dt)
{
    auto *playerArr = r.get_if<Player>();

    if (!playerArr) return;

    for (auto [player, ent] : zipper(*playerArr)) {
        if (player._cooldown > 0.f) {
            player._cooldown -= dt;
        }
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<Shoot>();
    }
}
