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

    // Loop through all players with invincibility component
    for (auto&& [player, invi, ent] : zipper(*playerArr, *inviArr)) {
        // Check if player is currently invincible
        if (invi._isInvincible) {
            // Update the timer
            invi._lastActivation += dt;

            // Check if invincibility duration has expired
            if (invi._lastActivation >= invi._duration) {
                invi._isInvincible = false;
                invi._lastActivation = 0.0f;
                std::cout << "Player invincibility ended!" << std::endl;
            }
        }
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<InvincibilitySys>();
    }
}
