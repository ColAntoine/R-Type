/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Enemy Spawn System Implementation
*/

#include "EnemySpawnSystem.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include <iostream>

EnemySpawnSystem::EnemySpawnSystem()
    : rng_(std::random_device{}()),
      type_dist_(1, 5),
      y_dist_(50.0f, 700.0f)
{
}

void EnemySpawnSystem::initialize_if_needed(registry& r) {
    if (initialized_) return;

    // Initialize y_dist_ with actual screen height from RenderManager
    auto& renderManager = RenderManager::instance();
    float screen_height = renderManager.get_screen_infos().getHeight();
    y_dist_ = std::uniform_real_distribution<>(50.0f, screen_height - 50.0f);

    initialized_ = true;
}

void EnemySpawnSystem::update(registry& r, float dt) {
    initialize_if_needed(r);

    auto* enemies = r.get_if<Enemy>();
    size_t current_count = enemies ? enemies->size() : 0;

    spawn_timer_ += dt;

    // Automatic spawning is disabled on clients — server is authoritative for enemy spawning.
    // Keep the logic here commented so it can be re-enabled easily if needed.
    if (spawn_timer_ >= spawn_interval_ && current_count < max_enemies_) {
        spawn_random_enemy(r);
        spawn_timer_ = 0.0f;
    }
}

entity EnemySpawnSystem::spawn_enemy(registry& r, uint8_t enemy_type, float x, float y) {
    entity e = r.spawn_entity();
    uint32_t net_id = next_network_id_++;

    r.emplace_component<position>(e, x, y);
    r.emplace_component<collider>(e, 65.0f, 132.0f, -32.5f, -66.0f, false);
    r.emplace_component<Enemy>(e, static_cast<Enemy::EnemyAIType>(enemy_type));
    r.emplace_component<Health>(e, 15); // DEFAULT VALUE, TO CHANGE LATER

    switch (static_cast<Enemy::EnemyAIType>(enemy_type)) {
        case Enemy::EnemyAIType::BASIC:
            r.emplace_component<animation>(e, std::string(RTYPE_PATH_ASSETS) + "enemy.gif", 65.0f, 132.0f, 1.f, 1.f, 8, false);
            r.emplace_component<velocity>(e, -80.0f, 0.0f);
            break;
        case Enemy::EnemyAIType::SINE_WAVE:
            r.emplace_component<animation>(e, std::string(RTYPE_PATH_ASSETS) + "enemy.gif", 65.0f, 132.0f, 1.f, 1.f, 8, false);
            r.emplace_component<velocity>(e, -60.0f, 0.0f);
            break;
        case Enemy::EnemyAIType::FAST:
            r.emplace_component<animation>(e, std::string(RTYPE_PATH_ASSETS) + "enemy.gif", 65.0f, 132.0f, 1.f, 1.f, 8, false);
            r.emplace_component<velocity>(e, -120.0f, 0.0f);
            break;
        case Enemy::EnemyAIType::ZIGZAG:
            r.emplace_component<animation>(e, std::string(RTYPE_PATH_ASSETS) + "enemy.gif", 65.0f, 132.0f, 1.f, 1.f, 8, false);
            r.emplace_component<velocity>(e, -70.0f, 50.0f);
            break;
        case Enemy::EnemyAIType::TURRET:
            r.emplace_component<animation>(e, std::string(RTYPE_PATH_ASSETS) + "enemy.gif", 65.0f, 132.0f, 1.f, 1.f, 8, false);
            r.emplace_component<velocity>(e, -80.0f, 0.0f);
            break;
    }

    return e;
}

void EnemySpawnSystem::spawn_random_enemy(registry& r) {
    auto& renderManager = RenderManager::instance();
    float screen_width = renderManager.get_screen_infos().getWidth();
    float screen_height = renderManager.get_screen_infos().getHeight();

    uint8_t enemy_type = type_dist_(rng_);
    float spawn_x = screen_width + 50.0f;
    float spawn_y = y_dist_(rng_);

    spawn_enemy(r, enemy_type, spawn_x, spawn_y);
}

void EnemySpawnSystem::set_world_bounds(float width, float height) {
    // World bounds are now managed by RenderManager
    // This method is kept for backward compatibility but is no longer used
    // Y distribution is updated based on screen height from RenderManager
    y_dist_ = std::uniform_real_distribution<>(50.0f, std::max(height - 50.0f, 100.0f));
}

std::unique_ptr<ISystem> create_system() {
    return std::make_unique<EnemySpawnSystem>();
}