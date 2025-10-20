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
    checkShoot(r);
}

void Shoot::checkShoot(registry &r)
{
    auto *playerArr = r.get_if<Player>();
    bool isShooting = IsKeyDown(KEY_SPACE);

    if (!playerArr) return;

    for (auto [player, ent] : zipper(*playerArr)) {
        if (isShooting) {
            player._isShooting = true;
            std::cout << "The player is shooting" << std::endl;
        }
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<Shoot>();
    }
}
