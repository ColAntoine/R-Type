#include "enemy_manager.hpp"
#include "udp_server.hpp"
#include "protocol.hpp"
#include <chrono>

// ECS and component types
#include "../../ECS/include/ECS/Registry.hpp"
#include "../../ECS/include/ECS/ComponentFactory.hpp"
#include "../Games/RType/Entity/Components/Enemy/Enemy.hpp"
#include "../Games/RType/Entity/Components/NetworkSync/NetworkSync.hpp"
#include "../Games/RType/Entity/Components/Health/Health.hpp"
#include "../Games/RType/Entity/Components/Drawable/Drawable.hpp"
#include "../../ECS/include/ECS/Components/Position.hpp"
#include "../../ECS/include/ECS/Components/Velocity.hpp"
#include "../../ECS/include/ECS/Components/Collider.hpp"

using namespace RType::Network;

ServerEnemyManager::ServerEnemyManager(std::shared_ptr<UdpServer> server, registry* ecs_registry, IComponentFactory* factory)
    : server_(server), ecs_registry_(ecs_registry), component_factory_(factory), rng_(std::random_device{}()), type_dist_(1, 4), y_dist_(50.0f, 718.0f)
{
}

ServerEnemyManager::~ServerEnemyManager() = default;

void ServerEnemyManager::set_world_bounds(float width, float height) {
    world_width_ = width;
    world_height_ = height;
    y_dist_ = std::uniform_real_distribution<float>(50.0f, height - 50.0f);
}

void ServerEnemyManager::update(float dt) {
    if (!server_ || !server_->is_running()) return;

    spawn_timer_ += dt;
    if (spawn_timer_ < spawn_interval_) return;
    spawn_timer_ = 0.0f;

    // Spawn a random enemy using server-side ECS (so server has authoritative entities)
    uint8_t enemy_type = static_cast<uint8_t>(type_dist_(rng_));
    float spawn_x = world_width_ + 50.0f;
    float spawn_y = y_dist_(rng_);
    uint32_t enemy_id = next_enemy_id_++;

    if (ecs_registry_ && component_factory_) {
        // Create server-side entity and components mirroring client EnemySpawnSystem
        entity e = ecs_registry_->spawn_entity();
        // network_sync id should match enemy_id so clients can map
        ecs_registry_->emplace_component<position>(e, spawn_x, spawn_y);
        ecs_registry_->emplace_component<collider>(e, 30.0f, 30.0f, -15.0f, -15.0f, false);
        ecs_registry_->emplace_component<Enemy>(e, static_cast<uint8_t>(enemy_type));
        ecs_registry_->emplace_component<network_sync>(e, enemy_id);
        ecs_registry_->emplace_component<Health>(e, 15);

        // visual and velocity vary by type
        switch (static_cast<int>(enemy_type)) {
            case 1:
                ecs_registry_->emplace_component<drawable>(e, 30.0f, 30.0f, 255, 0, 0, 255);
                ecs_registry_->emplace_component<velocity>(e, -80.0f, 0.0f);
                break;
            case 2:
                ecs_registry_->emplace_component<drawable>(e, 30.0f, 30.0f, 0, 255, 0, 255);
                ecs_registry_->emplace_component<velocity>(e, -60.0f, 0.0f);
                break;
            case 3:
                ecs_registry_->emplace_component<drawable>(e, 30.0f, 30.0f, 0, 0, 255, 255);
                ecs_registry_->emplace_component<velocity>(e, -120.0f, 0.0f);
                break;
            default:
                ecs_registry_->emplace_component<drawable>(e, 30.0f, 30.0f, 255, 165, 0, 255);
                ecs_registry_->emplace_component<velocity>(e, -70.0f, 0.0f);
                break;
        }
    }

    // Broadcast EntityCreate packet so clients create the enemy locally
    RType::Protocol::EntityCreate create_msg;
    create_msg.entity_id = enemy_id;
    create_msg.entity_type = enemy_type; // server uses types 1-4
    create_msg.x = spawn_x;
    create_msg.y = spawn_y;
    create_msg.health = 5.0f; // default health

    auto packet = RType::Protocol::create_packet(
        static_cast<uint8_t>(RType::Protocol::GameMessage::ENTITY_CREATE),
        create_msg
    );

    // Broadcast to all clients
    server_->broadcast(reinterpret_cast<const char*>(packet.data()), packet.size());
}
