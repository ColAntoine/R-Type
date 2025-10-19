/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** InvincibilitySys system implementation
*/

#include <memory>
#include <iostream>

#include "InvincibilitySys.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components.hpp"
#include "ECS/Zipper.hpp"

#include "Entity/Components/Player/Player.hpp"
#include "Entity/Components/Invincibility/Invincibility.hpp"

void InvincibilitySys::update(registry& r, float dt)
{
    checkPlayerInvicibilty(r, dt);
}

void InvincibilitySys::checkPlayerInvicibilty(registry &r, float dt)
{
    auto *playerArr = r.get_if<Player>();
    auto *inviArr = r.get_if<Invincibility>();

    if (!playerArr || !inviArr) return;

    for (auto&& [player, invi, ent] : zipper(*playerArr, *inviArr)) {
        activateInvi(invi, player);
        updatePlayerInvi(invi, dt);
    }
}

void InvincibilitySys::activateInvi(Invincibility &invi, Player &player)
{
    if (player._isHit) {
        player._isHit = false;
        invi._isInvincible = true;
        invi._lastActivation = 0.0f;
        std::cout << "Player invincibility activated!" << std::endl;
    }
}

void InvincibilitySys::updatePlayerInvi(Invincibility &invi, float dt)
{
    if (invi._isInvincible) {
        invi._lastActivation += dt;

        if (invi._lastActivation >= invi._duration) {
            invi._isInvincible = false;
            invi._lastActivation = 0.0f;
            std::cout << "Player invincibility ended!" << std::endl;
        }
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<InvincibilitySys>();
    }
}
