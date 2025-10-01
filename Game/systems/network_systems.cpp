#include "network_systems.hpp"
#include <iostream>

// NetworkSystem implementation
NetworkSystem::NetworkSystem(EventManager* event_manager, NetworkService* network_service)
    : event_manager_(event_manager), network_service_(network_service) {

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
    ecs_registry.add_component(remote_entity, position(x, y));
    ecs_registry.add_component(remote_entity, velocity(0.0f, 0.0f));
    ecs_registry.add_component(remote_entity, drawable(40.0f, 40.0f, 0, 100, 255, 255)); // Blue for remote players
    ecs_registry.add_component(remote_entity, collider(40.0f, 40.0f));
    ecs_registry.add_component(remote_entity, remote_player("Remote_" + std::to_string(player_id)));

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