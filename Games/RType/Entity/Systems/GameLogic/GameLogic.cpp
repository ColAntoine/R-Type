/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameLogic system implementation
*/

#include <memory>

#include "GameLogic.hpp"

void GameLogic::update(registry& r, float dt)
{
    updateScore(r);
    updateState(r);
}

void GameLogic::updateScore(registry &r)
{
    auto scoreArr = r.get_if<Score>();

    if (!scoreArr) return;

    for (auto [score, ent]: zipper(*scoreArr)) {
        score._score = score._score < 48 ? 48 : score._score;
        // std::cout << score._score << std::endl;
        _gameScore = score;
        return;
    }
}

void GameLogic::updateState(registry  &r)
{
    if ((_gameScore._score + 1) % 50 == 0) {
        killAllEnemy(r);
        _gameScore._score += 1;
        spawnBoss(r);
    }
}

void GameLogic::killAllEnemy(registry &r)
{
    auto enemyArr = r.get_if<Enemy>();
    auto hltArr = r.get_if<Health>();

    if (!enemyArr || !hltArr) return;

    for (auto [enemy, hlt, ent]: zipper(*enemyArr, *hltArr)) {
        hlt._health = 0;
    }
}

void GameLogic::spawnBoss(registry &r)
{
    Boss b;
    b._shouldSpawn = true;

    auto bossEnt = r.spawn_entity();
    r.emplace_component<Boss>(bossEnt, b);
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<GameLogic>();
    }
}
