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
    checkBallCollision(r);
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
    r.emplace_component<velocity>(ropeEntity, 0.0f, 0.0f);
    r.emplace_component<Rope>(ropeEntity, 3.0f, 800.0f, playerY, WHITE);
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
        rope._currentTipY -= rope._speed * dt;
        float ropeHeight = rope._startY - rope._currentTipY;
        col.h = ropeHeight;
        col.offset_y = -ropeHeight;

        if (rope._currentTipY <= 0) {
            ropesToRemove.push_back(entity(ent));
        }
    }

    for (auto& ropeEnt : ropesToRemove) {
        r.kill_entity(ropeEnt);
    }
}

static void killDeadEntities(registry &r, std::vector<entity> vEnt)
{
    for (auto &ent : vEnt) {
        r.kill_entity(entity(ent));
    }
}

void Shoot::checkBallCollision(registry &r)
{
    auto *ropeArr = r.get_if<Rope>();
    auto *ropePosArr = r.get_if<position>();
    auto *ropeColArr = r.get_if<collider>();

    auto *ballArr = r.get_if<Ball>();
    auto *ballPosArr = r.get_if<position>();

    if (!ropeArr || !ropePosArr || !ropeColArr || !ballArr || !ballPosArr) return;

    std::vector<entity> ropesToRemove;

    for (auto [rope, ropePos, ropeCol, ropeEnt] : zipper(*ropeArr, *ropePosArr, *ropeColArr)) {
        for (auto [ball, ballPos, ballEnt] : zipper(*ballArr, *ballPosArr)) {
            float ropeLeft = ropePos.x + ropeCol.offset_x;
            float ropeRight = ropeLeft + ropeCol.w;
            float ropeTop = ropePos.y + ropeCol.offset_y;
            float ropeBottom = ropeTop + ropeCol.h;

            float ballCenterX = ballPos.x;
            float ballCenterY = ballPos.y;

            float closestX = std::max(ropeLeft, std::min(ballCenterX, ropeRight));
            float closestY = std::max(ropeTop, std::min(ballCenterY, ropeBottom));

            float distanceX = ballCenterX - closestX;
            float distanceY = ballCenterY - closestY;
            float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

            // Check if collision occurred
            if (distanceSquared < (ball._radius * ball._radius)) {
                ball._isHit = true;
                ropesToRemove.push_back(entity(ropeEnt));
                break;
            }
        }
    }

    killDeadEntities(r, ropesToRemove);
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<Shoot>();
    }
}
