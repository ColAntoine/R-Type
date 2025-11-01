#include "EnemyHandler.hpp"
#include <iostream>
#include <cstring>
#include "Core/Server/Protocol/Protocol.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/Components/Animation.hpp"
#include "ECS/Components/Collider.hpp"
#include "Entity/Components/Enemy/Enemy.hpp"
#include "Entity/Components/Health/Health.hpp"
#include "Constants.hpp"

EnemyHandler::EnemyHandler(registry& registry, ILoader& loader)
    : registry_(registry), loader_(loader)
{
}

void EnemyHandler::on_enemy_spawn(const char* payload, size_t size) {
    using RType::Protocol::EntityCreate;

    if (!payload || size < sizeof(EntityCreate)) {
        std::cerr << "[EnemyHandler] Invalid ENTITY_CREATE payload" << std::endl;
        return;
    }

    EntityCreate ec;
    memcpy(&ec, payload, sizeof(ec));
    entity ent = registry_.spawn_entity();
    enemy_map_[ec.entity_id] = ent;

    float x = ec.x;
    float y = ec.y;
    uint8_t enemy_type = ec.entity_type;

    auto factory = loader_.get_factory();
    if (factory) {
        // Create position and collider
        factory->create_component<position>(registry_, ent, x, y);
        factory->create_component<collider>(registry_, ent, 65.0f, 132.0f, -32.5f, -66.0f, false);
        factory->create_component<Enemy>(registry_, ent, static_cast<Enemy::EnemyAIType>(enemy_type));
        factory->create_component<Health>(registry_, ent, 15);

        // Create animation based on enemy type (matching server EnemySpawnSystem logic)
        switch (static_cast<Enemy::EnemyAIType>(enemy_type)) {
            case Enemy::EnemyAIType::BASIC:
                factory->create_component<animation>(registry_, ent,
                    std::string(RTYPE_PATH_ASSETS) + "enemy.gif",
                    65.0f, 132.0f, 1.f, 1.f, 8, false);
                factory->create_component<velocity>(registry_, ent, -80.0f, 0.0f);
                break;
            case Enemy::EnemyAIType::SINE_WAVE:
                factory->create_component<animation>(registry_, ent,
                    std::string(RTYPE_PATH_ASSETS) + "enemy.gif",
                    65.0f, 132.0f, 1.f, 1.f, 8, false);
                factory->create_component<velocity>(registry_, ent, -60.0f, 0.0f);
                break;
            case Enemy::EnemyAIType::FAST:
                factory->create_component<animation>(registry_, ent,
                    std::string(RTYPE_PATH_ASSETS) + "enemy.gif",
                    65.0f, 132.0f, 1.f, 1.f, 8, false);
                factory->create_component<velocity>(registry_, ent, -120.0f, 0.0f);
                break;
            case Enemy::EnemyAIType::ZIGZAG:
                factory->create_component<animation>(registry_, ent,
                    std::string(RTYPE_PATH_ASSETS) + "enemy.gif",
                    65.0f, 132.0f, 1.f, 1.f, 8, false);
                factory->create_component<velocity>(registry_, ent, -70.0f, 50.0f);
                break;
            case Enemy::EnemyAIType::TURRET:
                factory->create_component<animation>(registry_, ent,
                    std::string(RTYPE_PATH_ASSETS) + "enemy.gif",
                    65.0f, 132.0f, 1.f, 1.f, 8, false);
                factory->create_component<velocity>(registry_, ent, -80.0f, 0.0f);
            break;
        }

        std::cout << "[EnemyHandler] Enemy spawned successfully with animation" << std::endl;
    } else {
        // Fallback without factory
        registry_.emplace_component<position>(ent, x, y);
        registry_.emplace_component<velocity>(ent, -80.0f, 0.0f);
        std::cerr << "[EnemyHandler] Factory not available, enemy spawned with minimal components" << std::endl;
    }
}
