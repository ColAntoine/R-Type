#pragma once

#include <memory>
#include <random>

// Forward declarations for ECS types to avoid pulling in heavy headers in this header
class registry;
class IComponentFactory;

namespace RType::Network {
    class UdpServer;

    class ServerEnemyManager {
    public:
        explicit ServerEnemyManager(std::shared_ptr<UdpServer> server, registry* ecs_registry, IComponentFactory* factory);
        ~ServerEnemyManager();

        // Update called every frame with delta time in seconds
        void update(float dt);

        // Configure world bounds for spawn Y
        void set_world_bounds(float width, float height);

    private:
        std::shared_ptr<UdpServer> server_;
        registry* ecs_registry_{nullptr};
        IComponentFactory* component_factory_{nullptr};
        std::mt19937 rng_;
        std::uniform_int_distribution<int> type_dist_;
        std::uniform_real_distribution<float> y_dist_;

        float spawn_timer_{0.0f};
        float spawn_interval_{1.0f};
        uint32_t next_enemy_id_{10001};
        float world_width_{1024.0f};
        float world_height_{768.0f};
    };
}
