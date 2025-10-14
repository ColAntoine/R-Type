#include "Core/Systems/Network/Network.hpp"
#include "Entity/Components/Drawable/Drawable.hpp"
#include "Entity/Components/Weapon/Weapon.hpp"
#include "Entity/Components/Controllable/Controllable.hpp"
#include "Entity/Components/Projectile/Projectile.hpp"
#include "Entity/Components/Health/Health.hpp"
#include "ECS/Zipper.hpp"
#include <iostream>

// NetworkSystem implementation
NetworkSystem::NetworkSystem(EventManager* event_manager, NetworkService* network_service, IComponentFactory* component_factory)
    : event_manager_(event_manager), network_service_(network_service), component_factory_(component_factory) {

    // Subscribe to network events
    event_manager_->subscribe<NetworkConnectedEvent>([this](const NetworkConnectedEvent& e) {
        handle_connected(e.player_id);
    });

    event_manager_->subscribe<PlayerJoinEvent>([this](const PlayerJoinEvent& e) {
        handle_player_join(e);
    });

    event_manager_->subscribe<PlayerShootEvent>([this](const PlayerShootEvent& e) {
        pending_remote_shoots_.push_back(e);
    });

    event_manager_->subscribe<PlayerLeaveEvent>([this](const PlayerLeaveEvent& e) {
        handle_player_leave(e);
    });

    event_manager_->subscribe<PlayerMoveEvent>([this](const PlayerMoveEvent& e) {
        handle_player_move(e);
    });

    event_manager_->subscribe<EnemySpawnEvent>([this](const EnemySpawnEvent& e) {
        handle_enemy_spawn(e);
    });

    event_manager_->subscribe<EnemyUpdateEvent>([this](const EnemyUpdateEvent& e) {
        handle_enemy_update(e);
    });

    event_manager_->subscribe<EnemyDestroyEvent>([this](const EnemyDestroyEvent& e) {
        handle_enemy_destroy(e);
    });

    event_manager_->subscribe<EntityCreateEvent>([this](const EntityCreateEvent& e) {
        handle_entity_create(e);
    });

    event_manager_->subscribe<EntityDestroyEvent>([this](const EntityDestroyEvent& e) {
        handle_entity_destroy(e);
    });
}

void NetworkSystem::update(registry& ecs_registry, __attribute_maybe_unused__ float delta_time) {
    // Process pending player joins
    for (const auto& join_event : pending_joins_) {
        auto it = remote_players_.find(join_event.player_id);
        if (it == remote_players_.end()) {
            entity remote_entity = create_remote_player(ecs_registry, join_event.player_id, join_event.x, join_event.y);
            remote_players_[join_event.player_id] = remote_entity;
        }
    }
    pending_joins_.clear();

    // Process pending player leaves
    for (const auto& leave_event : pending_leaves_) {
        auto it = remote_players_.find(leave_event.player_id);
        if (it != remote_players_.end()) {
            // TODO: Implement entity removal from registry
            remote_players_.erase(it);
        }
    }
    pending_leaves_.clear();

    // Process pending player moves
    for (const auto& move_event : pending_moves_) {
        auto it = remote_players_.find(move_event.player_id);
        if (it != remote_players_.end()) {
            update_remote_player_position(ecs_registry, move_event.player_id, move_event.x, move_event.y);
        } else {
            // Player not found - create them (fallback for missed JOIN message)
            entity remote_entity = create_remote_player(ecs_registry, move_event.player_id, move_event.x, move_event.y);
            remote_players_[move_event.player_id] = remote_entity;
        }
    }
    pending_moves_.clear();

    // Process pending enemy spawns
    for (const auto& spawn_event : pending_enemy_spawns_) {
        auto it = enemies_.find(spawn_event.enemy_id);
        if (it == enemies_.end()) {
            entity enemy_entity = create_enemy(ecs_registry, spawn_event.enemy_id, spawn_event.enemy_type,
                                             spawn_event.x, spawn_event.y, spawn_event.health);
            enemies_[spawn_event.enemy_id] = enemy_entity;
        }
    }
    pending_enemy_spawns_.clear();

    // Process pending enemy updates
    for (const auto& update_event : pending_enemy_updates_) {
        auto it = enemies_.find(update_event.enemy_id);
        if (it != enemies_.end()) {
            // update_enemy_position(ecs_registry, update_event.enemy_id, update_event.x, update_event.y);
        }
    }
    pending_enemy_updates_.clear();

    // Process pending enemy destroys
    for (const auto& destroy_event : pending_enemy_destroys_) {
        auto it = enemies_.find(destroy_event.enemy_id);
        if (it != enemies_.end()) {
            // TODO: Implement entity removal from registry
            enemies_.erase(it);
        }
    }
    pending_enemy_destroys_.clear();

    shoot_projectile(ecs_registry);
    // Detect and send any pending shoots
    send_pending_shoots(ecs_registry);

    // Send position updates periodically
    send_position_updates(ecs_registry);

}

void NetworkSystem::shoot_projectile(registry& ecs_registry) {
    for (const auto& shoot_event : pending_remote_shoots_) {
        entity projectile = ecs_registry.spawn_entity();
        component_factory_->create_component<Projectile>(ecs_registry, projectile, Projectile(shoot_event.player_id, 1, 400.0f, shoot_event.dir_x, shoot_event.dir_y, 5.0f, 4.0f, false));
        component_factory_->create_component<position>(ecs_registry, projectile, shoot_event.start_x, shoot_event.start_y);
        component_factory_->create_component<velocity>(ecs_registry, projectile, shoot_event.dir_x * 400.0f, shoot_event.dir_y * 400.0f);
        component_factory_->create_component<animation>(ecs_registry, projectile, "assets/Binary_bullet-Sheet.png", 220.0f, 220.0f, 0.1f, 0.1f, 0, false);
        component_factory_->create_component<lifetime>(ecs_registry, projectile, 5.0f, false, true);
    }
    pending_remote_shoots_.clear();
}

void NetworkSystem::handle_connected(int player_id) {
    // Player connected to server
    std::cout << "NetworkSystem: Local player " << player_id << " connected to server" << std::endl;
}

void NetworkSystem::handle_player_join(const PlayerJoinEvent& e) {
    std::cout << "NetworkSystem: Remote player " << e.player_id << " joined at (" << e.x << ", " << e.y << ")" << std::endl;
    pending_joins_.push_back(e);
}

void NetworkSystem::handle_player_leave(const PlayerLeaveEvent& e) {
    std::cout << "NetworkSystem: Remote player " << e.player_id << " left" << std::endl;
    pending_leaves_.push_back(e);
}

void NetworkSystem::handle_player_move(const PlayerMoveEvent& e) {
    pending_moves_.push_back(e);
}

void NetworkSystem::handle_entity_create(const EntityCreateEvent& e) {
    // Create entity (could be remote player, enemy, etc.)
    std::cout << "NetworkSystem: Entity " << e.entity_id << " created" << std::endl;
}

void NetworkSystem::handle_entity_destroy(const EntityDestroyEvent& e) {
    // Mark for removal
    pending_leaves_.push_back(PlayerLeaveEvent(e.entity_id));
}

entity NetworkSystem::create_remote_player(registry& ecs_registry, int player_id, float x, float y) {
    entity remote_entity = ecs_registry.spawn_entity();

    // Use factory methods instead of direct component creation
    component_factory_->create_component<position>(ecs_registry, remote_entity, x, y);
    component_factory_->create_component<velocity>(ecs_registry, remote_entity, 0.0f, 0.0f);
    component_factory_->create_component<collider>(ecs_registry, remote_entity, PLAYER_WIDTH, PLAYER_HEIGHT, -PLAYER_WIDTH/2.0f, -PLAYER_HEIGHT/2.0f);
    component_factory_->create_component<animation>(ecs_registry, remote_entity, "assets/spinning-skull-Sheet.png", 200, 200, 0.65, 0.5, 0, false);
    // component_factory_->create_remote_player(ecs_registry, remote_entity, "Remote_" + std::to_string(player_id));

    std::cout << "Created remote player entity " << remote_entity << " for player " << player_id << std::endl;
    return remote_entity;
}

void NetworkSystem::update_remote_player_position(registry& ecs_registry, int player_id, float x, float y) {
    auto it = remote_players_.find(player_id);
    if (it != remote_players_.end()) {
        auto* pos_array = ecs_registry.get_if<position>();
        if (pos_array && pos_array->has(static_cast<size_t>(it->second))) {
            pos_array->get(static_cast<size_t>(it->second)) = position(x, y);
        }
    }
}

void NetworkSystem::handle_enemy_spawn(const EnemySpawnEvent& e) {
    pending_enemy_spawns_.push_back(e);
}

void NetworkSystem::handle_enemy_update(const EnemyUpdateEvent& e) {
    pending_enemy_updates_.push_back(e);
}

void NetworkSystem::handle_enemy_destroy(const EnemyDestroyEvent& e) {
    std::cout << "NetworkSystem: Enemy " << e.enemy_id << " destroyed" << std::endl;
    pending_enemy_destroys_.push_back(e);
}

entity NetworkSystem::create_enemy(registry& ecs_registry, __attribute_maybe_unused__ int enemy_id, int enemy_type, float x, float y, __attribute_maybe_unused__ float health) {
    entity enemy_entity = ecs_registry.spawn_entity();
    component_factory_->create_component<position>(ecs_registry, enemy_entity, x, y);
    switch (enemy_type) {
        case 1:
            component_factory_->create_component<velocity>(ecs_registry, enemy_entity, -80.0f, 0.0f);
            break;
        case 2:
            component_factory_->create_component<velocity>(ecs_registry, enemy_entity, -60.0f, 0.0f);
            break;
        case 3:
            component_factory_->create_component<velocity>(ecs_registry, enemy_entity, -120.0f, 0.0f);
            break;
        default:
            component_factory_->create_component<velocity>(ecs_registry, enemy_entity, -70.0f, 0.0f);
            break;
    }
    component_factory_->create_component<Enemy>(ecs_registry, enemy_entity, static_cast<Enemy::EnemyAIType>(enemy_type));
    component_factory_->create_component<Health>(ecs_registry, enemy_entity, health);
    // Different enemy types have different appearance
    if (enemy_type == 1) {
        component_factory_->create_component<drawable>(ecs_registry, enemy_entity, 30.0f, 30.0f, 255, 0, 255, 255); // Magenta
    } else if (enemy_type == 2) {
        component_factory_->create_component<drawable>(ecs_registry, enemy_entity, 30.0f, 30.0f, 255, 255, 0, 255); // Yellow
    } else if (enemy_type == 3) {
        component_factory_->create_component<drawable>(ecs_registry, enemy_entity, 30.0f, 30.0f, 0, 255, 255, 255); // Cyan
    } else {
        component_factory_->create_component<drawable>(ecs_registry, enemy_entity, 30.0f, 30.0f, 255, 165, 0, 255); // Orange default
    }

    component_factory_->create_component<collider>(ecs_registry, enemy_entity, 30.0f, 30.0f, -15.0f, -15.0f);
    return enemy_entity;
}

void NetworkSystem::send_position_updates(registry& ecs_registry) {
    if (!network_service_->is_connected()) return;

    static int frame_counter = 0;
    if (++frame_counter >= 5) { // Send every 5 frames
        int local_player_id = network_service_->get_local_player_id();
        if (local_player_id > 0) {
            // Find local player entity (you'd need to track this properly)
            // For now, assume entity 0 is local player
            auto* pos_array = ecs_registry.get_if<position>();
            auto* vel_array = ecs_registry.get_if<velocity>();

            if (pos_array && vel_array && pos_array->size() > 0) {
                auto& pos = (*pos_array)[0];
                auto& vel = (*vel_array)[0];

                network_service_->send_player_position(pos.x, pos.y, vel.vx, vel.vy);
            }
        }
        frame_counter = 0;
    }
}

void NetworkSystem::send_pending_shoots(registry& ecs_registry) {
    if (!network_service_->is_connected()) return;

    auto* ctrlArr = ecs_registry.get_if<controllable>();
    auto* weaponArr = ecs_registry.get_if<Weapon>();
    auto* posArr = ecs_registry.get_if<position>();
    auto* colArr = ecs_registry.get_if<collider>();

    if (!ctrlArr || !weaponArr || !posArr) return;

    for (auto [ctrl, weapon, ent] : zipper(*ctrlArr, *weaponArr)) {
        if (!weapon._justFired) continue;
        if (!posArr->has(static_cast<size_t>(ent))) continue;

        float spawnX = posArr->get(static_cast<size_t>(ent)).x;
        float spawnY = posArr->get(static_cast<size_t>(ent)).y;

        if (colArr && colArr->has(static_cast<size_t>(ent))) {
            auto &col = colArr->get(static_cast<size_t>(ent));
            spawnX = spawnX + col.offset_x + col.w;
            spawnY = spawnY + col.offset_y + col.h / 2.0f;
        }

        float dirX = 1.0f;
        float dirY = 0.0f;
        uint8_t weapon_type = 0;

        network_service_->send_player_shoot(spawnX, spawnY, dirX, dirY, weapon_type);
        weapon._justFired = false;
    }
}
