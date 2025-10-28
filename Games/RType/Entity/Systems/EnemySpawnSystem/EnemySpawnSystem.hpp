/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Enemy Spawn System
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"
#include "ECS/Registry.hpp"
#include "ECS/ComponentFactory.hpp"

#include "Entity/Components/Enemy/Enemy.hpp"
#include "Entity/Components/Drawable/Drawable.hpp"

#include "ECS/ComponentFactory.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/Components/Collider.hpp"
#include "ECS/Components/Animation.hpp"

#include "Entity/Components/Health/Health.hpp"

#include "Constants.hpp"

#include <random>
#include <memory>
#include <functional>

// Callback type for broadcasting entity spawns to clients
using EnemySpawnCallback = std::function<void(entity, uint8_t, float, float)>;

class EnemySpawnSystem : public ISystem {
private:
    IComponentFactory* factory_{nullptr};
    uint32_t next_network_id_{10001};

    float spawn_timer_{0.0f};
    float spawn_interval_{3.0f};
    size_t max_enemies_{10};

    std::mt19937 rng_;
    std::uniform_int_distribution<> type_dist_;
    std::uniform_real_distribution<> y_dist_;

    bool initialized_{false};
    
    // Callback to notify when an enemy is spawned (for server to broadcast)
    EnemySpawnCallback spawn_callback_;

public:
    EnemySpawnSystem();

    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "EnemySpawnSystem"; }

    entity spawn_enemy(registry& r, uint8_t enemy_type, float x, float y);
    void spawn_random_enemy(registry& r);

    void set_spawn_interval(float interval) { spawn_interval_ = interval; }
    void set_max_enemies(size_t max) { max_enemies_ = max; }
    void set_world_bounds(float width, float height);
    
    // Seed the RNG with the registry's seed (for deterministic gameplay)
    void seed_from_registry(registry& r);
    
    // Set callback to be invoked when enemy spawns (for server broadcasting)
    void set_spawn_callback(EnemySpawnCallback callback) { spawn_callback_ = std::move(callback); }

    size_t get_max_enemies() const { return max_enemies_; }
    float get_spawn_interval() const { return spawn_interval_; }
private:
    void initialize_if_needed(registry& r);
};

// Global function to set enemy spawn callback (for server broadcasting)
void set_global_enemy_spawn_callback(EnemySpawnCallback callback);

extern "C" {
    std::unique_ptr<ISystem> create_system();
}