/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Shoot system implementation
*/

#include <memory>
#include <iostream>
#include <cmath>
#include <vector>

#include "Shoot.hpp"

void Shoot::update(registry& r, float dt) {
    updateCd(r, dt);
    checkShoot(r);
    updateRopes(r, dt);
}

void Shoot::checkShoot(registry &r)
{
    auto *playerArr = r.get_if<Player>();
    auto *posArr = r.get_if<position>();
    bool isShooting = IsKeyPressed(KEY_SPACE);

    if (!playerArr || !posArr) return;

    for (auto [player, pos, ent] : zipper(*playerArr, *posArr)) {
        if (isShooting && player._cooldown <= 0.f) {
            player._isShooting = true;
            player._cooldown = 2.f;
            spawnRope(r, pos.x, pos.y);
            std::cout << "Rope shot from position: (" << pos.x << ", " << pos.y << ")" << std::endl;
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

void Shoot::spawnRope(registry &r, float playerX, float playerY)
{
    auto ropeEntity = r.spawn_entity();
    // Position stays at player's location (the base of the rope)
    r.emplace_component<position>(ropeEntity, playerX, playerY);
    // No velocity needed - we'll update the tip position directly
    r.emplace_component<velocity>(ropeEntity, 0.0f, 0.0f);
    // Initialize rope with starting position
    r.emplace_component<Rope>(ropeEntity, 3.0f, 800.0f, playerY, WHITE);
    // Collider will be updated dynamically in updateRopes
    r.emplace_component<collider>(ropeEntity, 3.0f, 10.0f, -1.5f, 0.0f);
}

void Shoot::updateRopes(registry &r, float dt)
{
    auto *ropeArr = r.get_if<Rope>();
    auto *posArr = r.get_if<position>();
    auto *colliderArr = r.get_if<collider>();

    if (!ropeArr || !posArr || !colliderArr) return;

    std::vector<entity> ropesToRemove;

    for (auto [rope, pos, col, ent] : zipper(*ropeArr, *posArr, *colliderArr)) {
        // Move the tip of the rope upward
        rope._currentTipY -= rope._speed * dt;
        
        // Calculate the current height of the rope
        float ropeHeight = rope._startY - rope._currentTipY;
        
        // Update collider to match the rope's current height
        col.h = ropeHeight;
        col.offset_y = -ropeHeight; // Offset so collider extends upward from base
        
        // Check if rope reached the top of the screen
        if (rope._currentTipY <= 0) {
            ropesToRemove.push_back(entity(ent));
        }
    }

    for (auto& ropeEnt : ropesToRemove) {
        r.kill_entity(ropeEnt);
        std::cout << "Rope reached the top and was removed" << std::endl;
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<Shoot>();
    }
}
