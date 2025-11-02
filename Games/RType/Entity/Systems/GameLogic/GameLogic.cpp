/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameLogic system implementation
*/

#include <memory>

#include "GameLogic.hpp"
#include "ECS/Messaging/MessagingManager.hpp"

void GameLogic::update(registry& r, float dt)
{
    updateScore(r);
    updateState(r);
    checkPlayerDeath(r);
}

void GameLogic::updateScore(registry &r)
{
    auto scoreArr = r.get_if<Score>();

    if (!scoreArr) return;

    for (auto [score, ent]: zipper(*scoreArr)) {
        // * Uncomment if you want to debug boss
        // score._score = score._score < 50 ? 50 : score._score;
        _gameScore = score;
        return;
    }
}

void GameLogic::updateState(registry  &r)
{
    if (_gameScore._score != 0 && _gameScore._score % 25 == 0 && _lastBossSpawnScore != _gameScore._score) {
        _lastBossSpawnScore = _gameScore._score;
        killAllEnemy(r);
        _gameScore._score += 1;
        spawnBoss(r);
    }
}

void GameLogic::checkPlayerDeath(registry &r)
{
    auto playerArr = r.get_if<Player>();
    auto healthArr = r.get_if<Health>();

    if (!playerArr || !healthArr) return;

    bool allPlayersDead = true;

    // Check if all players (local and remote) are dead
    for (auto [player, health, ent]: zipper(*playerArr, *healthArr)) {
        if (health._health > 0) {
            allPlayersDead = false;
            break;
        }
    }

    // Emit game over event if all players are dead
    if (allPlayersDead && !_gameOverEmitted) {
        _gameOverEmitted = true;
        Event gameOverEvent("GAME_OVER");
        gameOverEvent.set("score", _gameScore._score);
        MessagingManager::instance().get_event_bus().emit(gameOverEvent);
        std::cout << "[GameLogic] GAME OVER! Final Score: " << _gameScore._score << std::endl;
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
    std::cout << "[GameLogic] Spawning Boss!" << std::endl;
    auto bossEnt = r.spawn_entity();
    r.emplace_component<Boss>(bossEnt, b);
}

DLL_EXPORT ISystem* create_system() {
    try {
        return new GameLogic();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}
