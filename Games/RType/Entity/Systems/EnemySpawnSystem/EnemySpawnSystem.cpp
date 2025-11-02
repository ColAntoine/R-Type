/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Enemy Spawn System Implementation
*/

#include "EnemySpawnSystem.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "Constants.hpp"
#include <iostream>

static EnemySpawnCallback g_enemy_spawn_callback = nullptr;

void set_global_enemy_spawn_callback(EnemySpawnCallback callback) {
    g_enemy_spawn_callback = std::move(callback);
}

EnemySpawnSystem::EnemySpawnSystem()
    : rng_(std::random_device{}()),
      type_dist_(1, 5),
      y_dist_(50.0f, 700.0f)
{
}

void EnemySpawnSystem::seed_from_registry(registry& r) {
    if (r.has_random_seed()) {
        rng_.seed(r.get_random_seed());
        std::cout << "[EnemySpawnSystem] Seeded RNG with: " << r.get_random_seed() << std::endl;
    }
}

void EnemySpawnSystem::initialize_if_needed(registry& r) {
    if (initialized_) return;

    auto& renderManager = RenderManager::instance();
    float screen_height = renderManager.get_screen_infos().getHeight();
    y_dist_ = std::uniform_real_distribution<>(50.0f, screen_height - 50.0f);

    seed_from_registry(r);

    initialized_ = true;
}

void EnemySpawnSystem::update(registry& r, float dt) {
    initialize_if_needed(r);

    int wave = getWave(r);
    float base_interval = 1.5f;
    size_t base_max = 10;
    spawn_interval_ = base_interval / (1.0f + wave * 0.15f);
    max_enemies_ = base_max + static_cast<size_t>(wave * 3);

    auto* enemies = r.get_if<Enemy>();
    size_t current_count = enemies ? enemies->size() : 0;

    spawn_timer_ += dt;

    if (spawn_timer_ >= spawn_interval_ && current_count < max_enemies_) {
        spawn_random_enemy(r);
        spawn_timer_ = 0.0f;
    }
}

entity EnemySpawnSystem::spawn_enemy(registry& r, uint8_t enemy_type, float x, float y, int wave) {
    entity e = r.spawn_entity();
    uint32_t net_id = next_network_id_++;

    auto& renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    float enemy_width = 65.0f;
    float enemy_height = 132.0f;
    float scale_x = GET_SCALE_X(enemy_width, winInfos.getWidth());
    float scale_y = GET_SCALE_Y(enemy_height, winInfos.getHeight());

    r.emplace_component<position>(e, x, y);
    r.emplace_component<collider>(e, scale_x, scale_y, -scale_x / 2.0f, -scale_y / 2.0f, false);
    r.emplace_component<Enemy>(e, static_cast<Enemy::EnemyAIType>(enemy_type));

    int base_health = 50;
    int health = base_health * (wave + 1);
    r.emplace_component<Health>(e, health);

    float speed_scale = 1.0f + wave * 0.1f;

    std::cout << "[EnemySpawnSystem] Spawning Wave " << wave << " Enemy Type " << (int)enemy_type 
              << " - Health: " << health << ", Speed Scale: " << speed_scale << std::endl;

    switch (static_cast<Enemy::EnemyAIType>(enemy_type)) {
        case Enemy::EnemyAIType::BASIC:
            r.emplace_component<animation>(e, std::string(RTYPE_PATH_ASSETS) + "enemy.gif", enemy_width, enemy_height, scale_x / enemy_width, scale_y / enemy_height, 8, false);
            r.emplace_component<velocity>(e, -80.0f * speed_scale, 0.0f);
            break;
        case Enemy::EnemyAIType::SINE_WAVE:
            r.emplace_component<animation>(e, std::string(RTYPE_PATH_ASSETS) + "enemy.gif", enemy_width, enemy_height, scale_x / enemy_width, scale_y / enemy_height, 8, false);
            r.emplace_component<velocity>(e, -60.0f * speed_scale, 0.0f);
            break;
        case Enemy::EnemyAIType::FAST:
            r.emplace_component<animation>(e, std::string(RTYPE_PATH_ASSETS) + "enemy.gif", enemy_width, enemy_height, scale_x / enemy_width, scale_y / enemy_height, 8, false);
            r.emplace_component<velocity>(e, -120.0f * speed_scale, 0.0f);
            break;
        case Enemy::EnemyAIType::ZIGZAG:
            r.emplace_component<animation>(e, std::string(RTYPE_PATH_ASSETS) + "enemy.gif", enemy_width, enemy_height, scale_x / enemy_width, scale_y / enemy_height, 8, false);
            r.emplace_component<velocity>(e, -70.0f * speed_scale, 50.0f * speed_scale);
            break;
        case Enemy::EnemyAIType::TURRET:
            r.emplace_component<animation>(e, std::string(RTYPE_PATH_ASSETS) + "enemy.gif", enemy_width, enemy_height, scale_x / enemy_width, scale_y / enemy_height, 8, false);
            r.emplace_component<velocity>(e, -80.0f * speed_scale, 0.0f);
            break;
    }

    if (spawn_callback_) {
        spawn_callback_(e, enemy_type, x, y);
    } else if (g_enemy_spawn_callback) {
        g_enemy_spawn_callback(e, enemy_type, x, y);
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

    if (shouldEnemySpawn(r)){
        int wave = getWave(r);
        spawn_enemy(r, enemy_type, spawn_x, spawn_y, wave);
    }
}

void EnemySpawnSystem::set_world_bounds(float width, float height) {
    y_dist_ = std::uniform_real_distribution<>(50.0f, std::max(height - 50.0f, 100.0f));
}

bool EnemySpawnSystem::shouldEnemySpawn(registry &r)
{
    auto bossArr = r.get_if<Boss>();

    return (!bossArr || bossArr->size() == 0);
}

int EnemySpawnSystem::getWave(registry &r)
{
    auto waveArr = r.get_if<CurrentWave>();

    if (!waveArr) return 0;

    for (auto [wave, ent]: zipper(*waveArr)) {
        return wave._currentWave;
    }
    return 0;
}

DLL_EXPORT ISystem* create_system() {
    try {
        return new EnemySpawnSystem();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}