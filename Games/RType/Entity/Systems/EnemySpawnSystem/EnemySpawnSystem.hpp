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
#include "Entity/Components/NetworkSync/NetworkSync.hpp"
#include "Entity/Components/Drawable/Drawable.hpp"
#include "ECS/ComponentFactory.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/Components/Collider.hpp"
#include "Entity/Components/Health/Health.hpp"
#include <random>
#include <memory>

class EnemySpawnSystem : public ISystem {
private:
    IComponentFactory* factory_{nullptr};
    uint32_t next_network_id_{10001};

    float spawn_timer_{0.0f};
    float spawn_interval_{3.0f};
    size_t max_enemies_{10};

    float world_width_{1024.0f};
    float world_height_{768.0f};

    std::mt19937 rng_;
    std::uniform_int_distribution<> type_dist_;
    std::uniform_real_distribution<> y_dist_;

    bool initialized_{false};

public:
    EnemySpawnSystem();

    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "EnemySpawnSystem"; }

    entity spawn_enemy(registry& r, uint8_t enemy_type, float x, float y);
    void spawn_random_enemy(registry& r);

    void set_spawn_interval(float interval) { spawn_interval_ = interval; }
    void set_max_enemies(size_t max) { max_enemies_ = max; }
    void set_world_bounds(float width, float height);

    size_t get_max_enemies() const { return max_enemies_; }
    float get_spawn_interval() const { return spawn_interval_; }
private:
    void initialize_if_needed(registry& r);
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}