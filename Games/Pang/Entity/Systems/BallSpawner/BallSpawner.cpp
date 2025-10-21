/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** BallSpawner system implementation
*/

#include <memory>
#include <iostream>

#include "BallSpawner.hpp"
#include "ECS/Components.hpp"

void BallSpawner::update(registry& r, float dt)
{
    getCurrentBalls(r);
    spawnNewBall(r, dt);
}

void BallSpawner::getCurrentBalls(registry &r)
{
    auto *ballArr = r.get_if<Ball>();

    if (!ballArr) {
        _currentBalls = 0;
        return;
    }

    _currentBalls = 0;
    for (auto [ball, ent] : zipper(*ballArr)) {
        _currentBalls++;
    }
}

void BallSpawner::spawnNewBall(registry &r, float dt)
{
    _spawnTimer += dt;

    bool needsSpawn = (_currentBalls < _minBalls) || (_spawnTimer >= _spawnInterval);

    if (!needsSpawn || _currentBalls > MAX_BALLS) return;

    _spawnTimer = 0.f;

    float ballRadius = 100.f;
    float minX = ballRadius;
    float maxX = SCREEN_WIDTH - ballRadius;
    float randomX = minX + static_cast<float>(std::rand()) / RAND_MAX * (maxX - minX);
    float randomVx = (std::rand() % 2 == 0) ? 1.f : -1.f;
    randomVx *= (50.f + static_cast<float>(std::rand()) / RAND_MAX * 100.f);
    position spawnPos(randomX, 100.f);
    Ball newBall;

    newBall.spawn(nullptr, r, spawnPos, LARGE, randomVx, 0.f);
    _currentBalls++;
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<BallSpawner>();
    }
}
