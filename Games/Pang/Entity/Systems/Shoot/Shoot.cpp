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
    r.emplace_component<position>(ropeEntity, playerX, playerY);
    r.emplace_component<velocity>(ropeEntity, 0.0f, -800.0f);
    r.emplace_component<Rope>(ropeEntity, 3.0f, 800.0f, 0.0f, WHITE);
    r.emplace_component<collider>(ropeEntity, 3.0f, 10.0f, -1.5f, -5.0f);
}

void Shoot::updateRopes(registry &r, float dt)
{
    auto *ropeArr = r.get_if<Rope>();
    auto *posArr = r.get_if<position>();
    auto *velArr = r.get_if<velocity>();

    if (!ropeArr || !posArr || !velArr) return;

    std::vector<entity> ropesToRemove;

    for (auto [rope, pos, vel, ent] : zipper(*ropeArr, *posArr, *velArr)) {
        // Check if rope reached the top of the screen
        if (pos.y <= 0) {
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
