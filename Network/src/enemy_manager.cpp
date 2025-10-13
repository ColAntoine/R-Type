#include "enemy_manager.hpp"
#include <iostream>
#include <random>
#include <cmath>

namespace RType::Network {

    EnemyManager::EnemyManager(std::shared_ptr<UdpServer> server)
        : server_(server), next_enemy_id_(10001), spawn_timer_(0.0f),
        spawn_interval_(3.0f), max_enemies_(10), world_width_(1024.0f), world_height_(768.0f) {
    }

    void EnemyManager::update(float delta_time) {
        // Only update if we should run game logic (at least one client connected and all are ready)
        if (!server_ || !server_->should_run_game_logic()) {
            return; // Don't update enemies if no clients are ready
        }

        // Update spawn timer
        spawn_timer_ += delta_time;

        // Spawn new enemies if conditions are met
        if (spawn_timer_ >= spawn_interval_ && enemies_.size() < max_enemies_) {
            // Random spawn logic
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_int_distribution<> type_dist(1, 4); // Enemy types 1-4
            static std::uniform_real_distribution<> y_dist(50.0f, world_height_ - 50.0f);

            uint8_t enemy_type = type_dist(gen);
            float spawn_x = world_width_ + 50.0f; // Spawn off-screen right
            float spawn_y = y_dist(gen);

            spawn_enemy(enemy_type, spawn_x, spawn_y);
            spawn_timer_ = 0.0f;
        }

        // Update all enemies
        std::vector<uint32_t> enemies_to_remove;
        for (auto& [enemy_id, enemy] : enemies_) {
            if (!enemy.active) {
                enemies_to_remove.push_back(enemy_id);
                continue;
            }

            // Update AI and movement
            update_enemy_ai(enemy, delta_time);
            update_enemy_movement(enemy, delta_time);

            // Check if enemy is out of bounds (left side)
            if (enemy.x < -100.0f) {
                enemies_to_remove.push_back(enemy_id);
                continue;
            }

            // Broadcast position update periodically
            enemy.ai_timer += delta_time;
            if (enemy.ai_timer >= 0.1f) { // Update 10 times per second
                broadcast_enemy_update(enemy);
                enemy.ai_timer = 0.0f;
            }
        }

        // Remove inactive/out-of-bounds enemies
        for (uint32_t enemy_id : enemies_to_remove) {
            destroy_enemy(enemy_id);
        }
    }

    void EnemyManager::spawn_enemy(uint8_t enemy_type, float x, float y) {
        uint32_t enemy_id = next_enemy_id_++;

        // Create enemy with type-specific properties
        Enemy enemy(enemy_id, enemy_type, x, y);

        // Set type-specific behavior
        switch (enemy_type) {
            case 1: // Basic straight-moving enemy
                enemy.vx = -80.0f; // Move left
                enemy.vy = 0.0f;
                enemy.health = 50.0f;
                break;
            case 2: // Sine wave enemy
                enemy.vx = -60.0f;
                enemy.vy = 0.0f;
                enemy.health = 75.0f;
                enemy.movement_pattern = 0.0f; // Used for sine wave
                break;
            case 3: // Fast enemy
                enemy.vx = -120.0f;
                enemy.vy = 0.0f;
                enemy.health = 25.0f;
                break;
            case 4: // Zigzag enemy
                enemy.vx = -70.0f;
                enemy.vy = 50.0f;
                enemy.health = 60.0f;
                break;
        }

        enemy.max_health = enemy.health;

        // Add to tracking
        // enemies_.emplace(enemy_id, enemy);

        // Broadcast spawn to all clients
        // broadcast_enemy_spawn(enemy);
    }

    void EnemyManager::destroy_enemy(uint32_t enemy_id) {
        auto it = enemies_.find(enemy_id);
        if (it != enemies_.end()) {
            // Broadcast destruction to all clients
            broadcast_enemy_destroy(enemy_id);
            enemies_.erase(it);
        }
    }

    void EnemyManager::clear_all_enemies() {
        for (const auto& [enemy_id, enemy] : enemies_) {
            broadcast_enemy_destroy(enemy_id);
        }
        enemies_.clear();
    }

    void EnemyManager::update_enemy_ai(Enemy& enemy, float delta_time) {
        // Type-specific AI behavior
        switch (enemy.type) {
            case 1: // Basic straight-moving enemy
                // No special AI, just moves straight
                break;

            case 2: // Sine wave enemy
                enemy.movement_pattern += delta_time * 3.0f; // Frequency
                enemy.vy = std::sin(enemy.movement_pattern) * 80.0f; // Amplitude
                break;

            case 3: // Fast enemy
                // No special AI, just moves fast
                break;

            case 4: // Zigzag enemy
                // Change direction periodically
                if (static_cast<int>(enemy.movement_pattern * 2.0f) % 2 == 0) {
                    enemy.vy = 50.0f;
                } else {
                    enemy.vy = -50.0f;
                }
                enemy.movement_pattern += delta_time;
                break;
        }
    }

    void EnemyManager::update_enemy_movement(Enemy& enemy, float delta_time) {
        // Update position based on velocity
        enemy.x += enemy.vx * delta_time;
        enemy.y += enemy.vy * delta_time;

        // Keep enemies within world bounds (Y axis)
        if (enemy.y < 50.0f) {
            enemy.y = 50.0f;
            if (enemy.vy < 0) enemy.vy = -enemy.vy; // Bounce
        }
        if (enemy.y > world_height_ - 50.0f) {
            enemy.y = world_height_ - 50.0f;
            if (enemy.vy > 0) enemy.vy = -enemy.vy; // Bounce
        }
    }

    void EnemyManager::broadcast_enemy_spawn(const Enemy& enemy) {
        if (!server_) return;

        Protocol::EntityCreate spawn_msg;
        spawn_msg.entity_id = enemy.id;
        spawn_msg.entity_type = enemy.type;
        spawn_msg.x = enemy.x;
        spawn_msg.y = enemy.y;
        spawn_msg.health = enemy.health;

        auto packet = Protocol::create_packet(
            static_cast<uint8_t>(Protocol::GameMessage::ENTITY_CREATE),
            spawn_msg
        );

        server_->broadcast(reinterpret_cast<const char*>(packet.data()), packet.size());
    }

    void EnemyManager::broadcast_enemy_update(const Enemy& enemy) {
        if (!server_) return;

        Protocol::PositionUpdate update_msg;
        update_msg.entity_id = enemy.id;
        update_msg.x = enemy.x;
        update_msg.y = enemy.y;
        update_msg.vx = enemy.vx;
        update_msg.vy = enemy.vy;
        update_msg.timestamp = static_cast<uint32_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());

        auto packet = Protocol::create_packet(
            static_cast<uint8_t>(Protocol::GameMessage::ENTITY_UPDATE),
            update_msg
        );

        server_->broadcast(reinterpret_cast<const char*>(packet.data()), packet.size());
    }

    void EnemyManager::broadcast_enemy_destroy(uint32_t enemy_id) {
        if (!server_) return;

        Protocol::EntityDestroy destroy_msg;
        destroy_msg.entity_id = enemy_id;
        destroy_msg.reason = 0; // Normal destruction

        auto packet = Protocol::create_packet(
            static_cast<uint8_t>(Protocol::GameMessage::ENTITY_DESTROY),
            destroy_msg
        );

        server_->broadcast(reinterpret_cast<const char*>(packet.data()), packet.size());
    }

} // namespace RType::Network