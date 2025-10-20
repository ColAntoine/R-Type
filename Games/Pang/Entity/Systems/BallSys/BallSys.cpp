/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** BallSys system implementation
*/

#include <memory>
#include <iostream>
#include <cmath>
#include <vector>

#include "BallSys.hpp"
#include "Entity/Components/Gravity/Gravity.hpp"

#include "ECS/Zipper.hpp"
#include "Constants.hpp"

void BallSys::update(registry& r, float dt) {
    bounceBalls(r);
    checkBallCollisions(r);
    checkPlayerHits(r);
    splitBalls(r);
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
        if (pos.y <= 0 + ball._radius) {
            pos.y = 0 + ball._radius;
            vel.vy = std::abs(vel.vy);
        }
        if (pos.x >= SCREEN_WIDTH - ball._radius) {
            pos.x = SCREEN_WIDTH - ball._radius;
            vel.vx = -std::abs(vel.vx);
        }
        if (pos.x <= 0 + ball._radius) {
            pos.x = 0 + ball._radius;
            vel.vx = std::abs(vel.vx);
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
        for (auto&& [ballPos, ball, ballVel, ballEnt] : zipper(*ballPosArr, *ballArr, *ballVelArr)) {
            if (checkPlayerColision(playerPos, ballPos, ball)) {
                player._isHit = true;
                computeBallPhysics(ballPos, playerPos, ball, ballVel, playerVel);
            }
        }
    }
}

void BallSys::computeBallPhysics(position &ballPos, position &playerPos, Ball &ball,
    velocity &ballVel, velocity &playerVel)
{
    float ballCenterX = ballPos.x;
    float ballCenterY = ballPos.y;
    float playerCenterX = playerPos.x;
    float playerCenterY = playerPos.y;
    float playerLeft = playerPos.x -50.0f;
    float playerRight = playerPos.x + 50.0f;
    float playerTop = playerPos.y - 50.0f;
    float playerBottom = playerPos.y + 50.0f;

    // Determine which side of the player was hit
    float dx = ballCenterX - playerCenterX;
    float dy = ballCenterY - playerCenterY;

    if (std::abs(dx) > std::abs(dy)) {
        // Hit from side
        if (dx > 0) {
            // Ball is on the right side of player
            ballPos.x = playerRight + ball._radius;
            ballVel.vx = std::abs(playerVel.vx) > 0 ? playerVel.vx : 100.0f; // Push right
        } else {
            // Ball is on the left side of player
            ballPos.x = playerLeft - ball._radius;
            ballVel.vx = std::abs(playerVel.vx) > 0 ? playerVel.vx : -100.0f; // Push left
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

bool BallSys::checkPlayerColision(position &playerPos, position &ballPos, Ball &ball)
{
    float playerLeft = playerPos.x -50.0f;
    float playerRight = playerPos.x + 50.0f;
    float playerTop = playerPos.y - 50.0f;
    float playerBottom = playerPos.y + 50.0f;

    float ballLeft = ballPos.x - ball._radius;
    float ballRight = ballPos.x + ball._radius;
    float ballTop = ballPos.y - ball._radius;
    float ballBottom = ballPos.y + ball._radius;

    return !(ballRight < playerLeft ||
            ballLeft > playerRight ||
            ballBottom < playerTop ||
            ballTop > playerBottom);
}

void BallSys::checkBallCollisions(registry &r)
{
    auto *ballPosArr = r.get_if<position>();
    auto *ballArr = r.get_if<Ball>();
    auto *ballVelArr = r.get_if<velocity>();

    if (!ballPosArr || !ballArr || !ballVelArr) return;

    std::vector<std::tuple<position*, Ball*, velocity*, std::size_t>> balls;
    for (auto&& [ball, pos, vel, ent] : zipper(*ballArr, *ballPosArr, *ballVelArr)) {
        balls.push_back(std::make_tuple(&pos, &ball, &vel, ent));
    }

    for (size_t i = 0; i < balls.size(); ++i) {
        for (size_t j = i + 1; j < balls.size(); ++j) {
            auto& [pos1, ball1, vel1, ent1] = balls[i];
            auto& [pos2, ball2, vel2, ent2] = balls[j];

            if (checkBallToBallCollision(*pos1, *pos2, *ball1, *ball2)) {
                resolveBallCollision(*pos1, *pos2, *ball1, *ball2, *vel1, *vel2);
            }
        }
    }
}

bool BallSys::checkBallToBallCollision(position &ball1Pos, position &ball2Pos, Ball &ball1, Ball &ball2)
{
    float dx = ball2Pos.x - ball1Pos.x;
    float dy = ball2Pos.y - ball1Pos.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    float minDistance = ball1._radius + ball2._radius;

    return distance < minDistance;
}

void BallSys::resolveBallCollision(position &ball1Pos, position &ball2Pos, Ball &ball1, Ball &ball2,
    velocity &ball1Vel, velocity &ball2Vel)
{
    float dx = ball2Pos.x - ball1Pos.x;
    float dy = ball2Pos.y - ball1Pos.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance == 0) {
        distance = 0.01f;
        dx = 1.0f;
        dy = 0.0f;
    }

    float nx = dx / distance;
    float ny = dy / distance;

    float overlap = (ball1._radius + ball2._radius) - distance;
    float separationX = (overlap / 2.0f) * nx;
    float separationY = (overlap / 2.0f) * ny;

    ball1Pos.x -= separationX;
    ball1Pos.y -= separationY;
    ball2Pos.x += separationX;
    ball2Pos.y += separationY;

    float relVelX = ball1Vel.vx - ball2Vel.vx;
    float relVelY = ball1Vel.vy - ball2Vel.vy;

    float velAlongNormal = relVelX * nx + relVelY * ny;

    if (velAlongNormal > 0) {
        return;
    }

    float restitution = 0.5f;
    float impulse = -(1.0f + restitution) * velAlongNormal / 2.0f;
    float impulseX = impulse * nx;
    float impulseY = impulse * ny;

    ball1Vel.vx -= impulseX;
    ball1Vel.vy -= impulseY;
    ball2Vel.vx += impulseX;
    ball2Vel.vy += impulseY;
}

void BallSys::splitBalls(registry &r)
{
    auto *ballArr = r.get_if<Ball>();
    auto *posArr = r.get_if<position>();
    auto *velArr = r.get_if<velocity>();

    if (!ballArr || !posArr || !velArr) return;

    std::vector<std::tuple<Ball, position, velocity, entity>> ballsToSplit;

    for (auto [ball, pos, vel, ent]: zipper(*ballArr, *posArr, *velArr)) {
        if (ball._isHit) {
            ballsToSplit.push_back(std::make_tuple(ball, pos, vel, entity(ent)));
        }
    }

    // Process each ball that was hit
    for (auto& [ball, pos, vel, ent] : ballsToSplit) {
        if (ball._type == SMALL) {
            r.kill_entity(ent);
        } else {
            ballType newType = (ball._type == LARGE) ? MEDIUM : SMALL;
            float newRadius = (newType == MEDIUM) ? 50.f : 25.f;
            Ball newBall;
            newBall.spawn(nullptr, r, position(pos.x - newRadius, pos.y), newType, -150.0f, vel.vy * 0.5f);
            newBall.spawn(nullptr, r, position(pos.x + newRadius, pos.y), newType, 150.0f, vel.vy * 0.5f);

            r.kill_entity(ent);
        }
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<BallSys>();
    }
}
