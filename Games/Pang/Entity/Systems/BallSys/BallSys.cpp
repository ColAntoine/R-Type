/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** BallSys system implementation
*/

#include <memory>
#include <iostream>
#include <cmath>

#include "BallSys.hpp"

#include "ECS/Components.hpp"
#include "Entity/Components/Ball/Ball.hpp"
#include "Entity/Components/Player/Player.hpp"

#include "ECS/Zipper.hpp"
#include "Constants.hpp"

void BallSys::update(registry& r, float dt) {
    bounceBalls(r);
    checkPlayerHits(r);
}

void BallSys::bounceBalls(registry &r)
{
    auto ballArr = r.get_if<Ball>();
    auto velArr = r.get_if<velocity>();
    auto posArr = r.get_if<position>();

    if (!ballArr || !velArr || !posArr) return;

    for (auto&& [ball, vel, pos, ent]: zipper(*ballArr, *velArr, *posArr)) {
        if (pos.y >= SCREEN_HEIGHT - ball._radius) {
            pos.y = SCREEN_HEIGHT - ball._radius;
            vel.vy = -std::abs(vel.vy);
        }
        if (pos.x >= SCREEN_WIDTH - ball._radius) {
            pos.x = SCREEN_WIDTH - ball._radius;
            vel.vx = -std::abs(vel.vy);
        }
        if (pos.x <= 0 + ball._radius) {
            pos.x = 0 + ball._radius;
            vel.vx = std::abs(vel.vy);
        }
    }
}

void BallSys::checkPlayerHits(registry &r)
{
    auto *ballPosArr = r.get_if<position>();
    auto *ballArr = r.get_if<Ball>();
    auto *ballVelArr = r.get_if<velocity>();

    auto *playerPosArr = r.get_if<position>();
    auto *playerVelArr = r.get_if<velocity>();
    auto *playerArr = r.get_if<Player>();

    if (!ballPosArr || !ballArr || !ballVelArr || !playerPosArr || !playerArr) return;

    for (auto&& [playerPos, playerVel, player, playerEnt] : zipper(*playerPosArr, *playerVelArr, *playerArr)) {
        float playerLeft = playerPos.x -50.0f;
        float playerRight = playerPos.x + 50.0f;
        float playerTop = playerPos.y - 50.0f;
        float playerBottom = playerPos.y + 50.0f;

        for (auto&& [ballPos, ball, ballVel, ballEnt] : zipper(*ballPosArr, *ballArr, *ballVelArr)) {
            float ballLeft = ballPos.x - ball._radius;
            float ballRight = ballPos.x + ball._radius;
            float ballTop = ballPos.y - ball._radius;
            float ballBottom = ballPos.y + ball._radius;

            bool collision = !(ballRight < playerLeft ||
                              ballLeft > playerRight ||
                              ballBottom < playerTop ||
                              ballTop > playerBottom);

            if (collision) {
                std::cout << "Ball hit player! Ball at (" << ballPos.x << ", " << ballPos.y 
                          << ") Player at (" << playerPos.x << ", " << playerPos.y << ")" << std::endl;

                player._invincibility = 2.0f;
                // Calculate collision response
                float ballCenterX = ballPos.x;
                float ballCenterY = ballPos.y;
                float playerCenterX = playerPos.x;
                float playerCenterY = playerPos.y;

                // Determine which side of the player was hit
                float dx = ballCenterX - playerCenterX;
                float dy = ballCenterY - playerCenterY;

                if (std::abs(dx) > std::abs(dy)) {
                    // Hit from side
                    ballVel.vx = -ballVel.vx;
                    if (dx > 0) {
                        ballPos.x = playerRight + ball._radius;
                        ballVel.vx = playerVel.vx;
                    } else {
                        ballPos.x = playerLeft - ball._radius;
                        ballVel.vx = -playerVel.vx;
                    }
                } else {
                    // Hit from top/bottom
                    ballVel.vy = -ballVel.vy;
                    if (dy > 0) {
                        ballPos.y = playerBottom + ball._radius;
                    } else {
                        ballPos.y = playerTop - ball._radius;
                    }
                }
            }
        }
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<BallSys>();
    }
}
