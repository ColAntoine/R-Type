#include "Core/Systems/Network/Network.hpp"
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

void NetworkSystem::update(registry& ecs_registry, float delta_time) {
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
            update_enemy_position(ecs_registry, update_event.enemy_id, update_event.x, update_event.y);
            update_enemy_health(ecs_registry, update_event.enemy_id, update_event.health);
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

    // Send position updates periodically
    send_position_updates(ecs_registry);
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
    component_factory_->create_position(ecs_registry, remote_entity, x, y);
    component_factory_->create_velocity(ecs_registry, remote_entity, 0.0f, 0.0f);
    component_factory_->create_drawable(ecs_registry, remote_entity, PLAYER_WIDTH, PLAYER_HEIGHT, 0, 100, 255, 255); // Blue for remote players
    component_factory_->create_collider(ecs_registry, remote_entity, PLAYER_WIDTH, PLAYER_HEIGHT);
    component_factory_->create_remote_player(ecs_registry, remote_entity, "Remote_" + std::to_string(player_id));

    std::cout << "Created remote player entity " << remote_entity << " for player " << player_id << std::endl;
    return remote_entity;
}

void NetworkSystem::update_remote_player_position(registry& ecs_registry, int player_id, float x, float y) {
    auto it = remote_players_.find(player_id);
    if (it != remote_players_.end()) {
        auto* pos_array = ecs_registry.get_if<position>();
        if (pos_array && pos_array->size() > static_cast<size_t>(it->second)) {
            (*pos_array)[static_cast<size_t>(it->second)] = position(x, y);
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

entity NetworkSystem::create_enemy(registry& ecs_registry, int enemy_id, int enemy_type, float x, float y, float health) {
    entity enemy_entity = ecs_registry.spawn_entity();

    // Use factory methods instead of direct component creation
    component_factory_->create_position(ecs_registry, enemy_entity, x, y);
    component_factory_->create_velocity(ecs_registry, enemy_entity, 0.0f, 0.0f);

    // Different enemy types have different appearance
    if (enemy_type == 1) {
        component_factory_->create_drawable(ecs_registry, enemy_entity, 30.0f, 30.0f, 255, 0, 255, 255); // Magenta
    } else if (enemy_type == 2) {
        component_factory_->create_drawable(ecs_registry, enemy_entity, 30.0f, 30.0f, 255, 255, 0, 255); // Yellow
    } else if (enemy_type == 3) {
        component_factory_->create_drawable(ecs_registry, enemy_entity, 30.0f, 30.0f, 0, 255, 255, 255); // Cyan
    } else {
        component_factory_->create_drawable(ecs_registry, enemy_entity, 30.0f, 30.0f, 255, 165, 0, 255); // Orange default
    }

    component_factory_->create_collider(ecs_registry, enemy_entity, 30.0f, 30.0f);
    component_factory_->create_enemy(ecs_registry, enemy_entity, enemy_type, health);

    std::cout << "Created enemy entity " << enemy_entity << " for enemy " << enemy_id << " (type " << enemy_type << ")" << std::endl;
    return enemy_entity;
}

void NetworkSystem::update_enemy_position(registry& ecs_registry, int enemy_id, float x, float y) {
    auto it = enemies_.find(enemy_id);
    if (it != enemies_.end()) {
        auto* pos_array = ecs_registry.get_if<position>();
        if (pos_array && pos_array->size() > static_cast<size_t>(it->second)) {
            (*pos_array)[static_cast<size_t>(it->second)] = position(x, y);
        }
    }
}

void NetworkSystem::update_enemy_health(registry& ecs_registry, int enemy_id, float health) {
    auto it = enemies_.find(enemy_id);
    if (it != enemies_.end()) {
        auto* enemy_array = ecs_registry.get_if<enemy>();
        if (enemy_array && enemy_array->size() > static_cast<size_t>(it->second)) {
            auto& enemy_comp = (*enemy_array)[static_cast<size_t>(it->second)];
            enemy_comp.health = health;
        }
    }
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