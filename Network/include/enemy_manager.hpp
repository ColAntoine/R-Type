#pragma once

#include "protocol.hpp"
#include "udp_server.hpp"
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>

namespace RType::Network {

    /**
     * @brief Represents an enemy entity on the server
     */
    struct Enemy {
        uint32_t id;
        uint8_t type;
        float x, y;           // Position
        float vx, vy;         // Velocity
        float health;
        float max_health;

        // AI behavior
        float ai_timer;
        float movement_pattern;
        bool active;

        Enemy(uint32_t id_, uint8_t type_, float x_, float y_)
            : id(id_), type(type_), x(x_), y(y_), vx(0), vy(0),
              health(100.0f), max_health(100.0f), ai_timer(0.0f),
              movement_pattern(0.0f), active(true) {}
    };

    /**
     * @brief Manages enemy spawning, AI, and lifecycle on the server
     */
    class EnemyManager {
        private:
            std::shared_ptr<UdpServer> server_;
            std::unordered_map<uint32_t, Enemy> enemies_;

            // Spawning configuration
            uint32_t next_enemy_id_;
            float spawn_timer_;
            float spawn_interval_;
            size_t max_enemies_;

            // Game bounds
            float world_width_;
            float world_height_;

        public:
            EnemyManager(std::shared_ptr<UdpServer> server);
            ~EnemyManager() = default;

            // Main update loop - called from server main loop
            void update(float delta_time);

            // Enemy lifecycle
            void spawn_enemy(uint8_t enemy_type, float x, float y);
            void destroy_enemy(uint32_t enemy_id);
            void clear_all_enemies();

            // AI and movement
            void update_enemy_ai(Enemy& enemy, float delta_time);
            void update_enemy_movement(Enemy& enemy, float delta_time);

            // Network broadcasting
            void broadcast_enemy_spawn(const Enemy& enemy);
            void broadcast_enemy_update(const Enemy& enemy);
            void broadcast_enemy_destroy(uint32_t enemy_id);

            // Configuration
            void set_spawn_interval(float interval) { spawn_interval_ = interval; }
            void set_max_enemies(size_t max) { max_enemies_ = max; }
            void set_world_bounds(float width, float height) { world_width_ = width; world_height_ = height; }

            // Status
            size_t get_enemy_count() const { return enemies_.size(); }
            size_t get_max_enemies() const { return max_enemies_; }
            float get_spawn_interval() const { return spawn_interval_; }
            bool has_enemy(uint32_t enemy_id) const { return enemies_.find(enemy_id) != enemies_.end(); }
    };

} // namespace RType::Network