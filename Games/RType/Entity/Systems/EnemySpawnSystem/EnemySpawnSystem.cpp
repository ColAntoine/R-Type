/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Enemy Spawn System Implementation
*/

#include "EnemySpawnSystem.hpp"
#include <iostream>

EnemySpawnSystem::EnemySpawnSystem()
    : rng_(std::random_device{}()),
      type_dist_(1, 4),
      y_dist_(50.0f, 718.0f)
{
}

void EnemySpawnSystem::initialize_if_needed(registry& r) {
    if (initialized_) return;

    // Retrieve factory from the registry (via DLLoader)
    // Note: We assume the registry has a method to access the factory
    // Otherwise, the factory must be passed to the constructor

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
    r.emplace_component<collider>(e, 30.0f, 30.0f, -15.0f, -15.0f, false);
    r.emplace_component<Enemy>(e, static_cast<Enemy::EnemyAIType>(enemy_type));
    r.emplace_component<Health>(e, 15); // DEFAULT VALUE, TO CHANGE LATER

    switch (static_cast<Enemy::EnemyAIType>(enemy_type)) {
        case Enemy::EnemyAIType::BASIC:
            r.emplace_component<drawable>(e, 30.0f, 30.0f, 255, 0, 0, 255);
            r.emplace_component<velocity>(e, -80.0f, 0.0f);
            break;
        case Enemy::EnemyAIType::SINE_WAVE:
            r.emplace_component<drawable>(e, 30.0f, 30.0f, 0, 255, 0, 255);
            r.emplace_component<velocity>(e, -60.0f, 0.0f);
            break;
        case Enemy::EnemyAIType::FAST:
            r.emplace_component<drawable>(e, 30.0f, 30.0f, 0, 0, 255, 255);
            r.emplace_component<velocity>(e, -120.0f, 0.0f);
            break;
        case Enemy::EnemyAIType::ZIGZAG:
            r.emplace_component<drawable>(e, 30.0f, 30.0f, 255, 255, 0, 255);
            r.emplace_component<velocity>(e, -70.0f, 50.0f);
            break;
    }

    return e;
}

void EnemySpawnSystem::spawn_random_enemy(registry& r) {
    uint8_t enemy_type = type_dist_(rng_);
    float spawn_x = world_width_ + 50.0f;
    float spawn_y = y_dist_(rng_);

    spawn_enemy(r, enemy_type, spawn_x, spawn_y);
}

void EnemySpawnSystem::set_world_bounds(float width, float height) {
    world_width_ = width;
    world_height_ = height;
    y_dist_ = std::uniform_real_distribution<>(50.0f, height - 50.0f);
}

std::unique_ptr<ISystem> create_system() {
    return std::make_unique<EnemySpawnSystem>();
}