/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Enemy AI System Implementation
*/

#include "EnemyAI.hpp"
#include <cmath>

void EnemyAISystem::update(registry& r, float dt) {
    auto* enemies = r.get_if<enemy>();
    auto* velocities = r.get_if<velocity>();
    auto* positions = r.get_if<position>();

    if (!enemies || !velocities || !positions) return;

    for (auto [enm, vel, pos, entity] : zipper(*enemies, *velocities, *positions)) {
        enm.timer += dt;
        switch (enm.enemy_type) {
            case EnemyAIType::BASIC:
                break;
            case EnemyAIType::SINE_WAVE:
                enm.movement_pattern += dt * 3.0f; // Frequency
                vel.vy = std::sin(enm.movement_pattern) * 80.0f; // Amplitude
                break;
            case EnemyAIType::FAST:
                break;
            case EnemyAIType::ZIGZAG:
                enm.movement_pattern += dt;
                if (static_cast<int>(enm.movement_pattern * 2.0f) % 2 == 0) {
                    if (vel.vy < 0) vel.vy = -vel.vy;
                } else {
                    if (vel.vy > 0) vel.vy = -vel.vy;
                }
                break;
        }
        if (pos.y < 50.0f && vel.vy < 0) {
            vel.vy = -vel.vy;
        }
        if (pos.y > world_height_ - 50.0f && vel.vy > 0) {
            vel.vy = -vel.vy;
        }
    }
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<EnemyAISystem>();
    }
}