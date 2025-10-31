/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Enemy AI System Implementation
*/

#include "EnemyAI.hpp"
#include <cmath>
#include "ECS/Renderer/RenderManager.hpp"

void EnemyAISystem::update(registry& r, float dt) {
    auto* enemies = r.get_if<Enemy>();
    auto* velocities = r.get_if<velocity>();
    auto* positions = r.get_if<position>();

    if (!enemies || !velocities || !positions) return;

    for (auto [enm, vel, pos, ent] : zipper(*enemies, *velocities, *positions)) {
        enm.timer += dt;
        switch (enm.enemy_type) {
            case Enemy::EnemyAIType::BASIC:
                break;
            case Enemy::EnemyAIType::SINE_WAVE:
                enm.movement_pattern += dt * 3.0f;
                vel.vy = std::sin(enm.movement_pattern) * 80.0f;
                break;
            case Enemy::EnemyAIType::FAST:
                break;
            case Enemy::EnemyAIType::ZIGZAG:
                enm.movement_pattern += dt;
                if (static_cast<int>(enm.movement_pattern * 2.0f) % 2 == 0) {
                    if (vel.vy < 0) vel.vy = -vel.vy;
                } else {
                    if (vel.vy > 0) vel.vy = -vel.vy;
                }
                break;
            case Enemy::EnemyAIType::TURRET:
            turretEnnemyAi(enm, vel, pos, dt, entity(ent), r);
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

void EnemyAISystem::turretEnnemyAi(Enemy &enm, velocity &vel, position &pos, float dt, entity ent, registry &r)
{
    auto &rm = RenderManager::instance();
    float screen_width = rm.get_screen_infos().getWidth();
    float target_x = screen_width * 0.75f;

    if (pos.x > target_x) {
        if (vel.vx > 0) vel.vx = -std::abs(vel.vx);
    } else {
        auto *weaponArr = r.get_if<Weapon>();
        size_t ent_id = static_cast<size_t>(ent);
        if (!weaponArr || !weaponArr->has(ent_id)) {
            Weapon w(ent, std::string("enemy"), 1.0f, 10, 300.0f, -1, true);
            w._wantsToFire = true;
            w._automatic = true;
            r.emplace_component<Weapon>(ent, w);
        }
        vel.vx = 0.0f;
        pos.x = target_x;
        vel.vy = 0.0f;
        enm.timer = 0.0f;
    }
}

DLL_EXPORT ISystem* create_system() {
    try {
        return new EnemyAISystem();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}