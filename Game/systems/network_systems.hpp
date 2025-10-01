#pragma once

#include "core/event_manager.hpp"
#include "core/events.hpp"
#include "services/input_service.hpp"
#include "services/network_service.hpp"
#include "ecs/registry.hpp"
#include "ecs/components.hpp"
#include "ecs/entity.hpp"
#include <unordered_map>

// ECS System that handles network events
class NetworkSystem {
    private:
        EventManager* event_manager_;
        NetworkService* network_service_;
        std::unordered_map<int, entity> remote_players_;

        // Pending operations (to be processed in update with registry access)
        std::vector<PlayerJoinEvent> pending_joins_;
        std::vector<PlayerLeaveEvent> pending_leaves_;
        std::vector<PlayerMoveEvent> pending_moves_;

    public:
            NetworkSystem(EventManager* event_manager, NetworkService* network_service);
            void update(registry& ecs_registry, float delta_time);

        private:
            void handle_connected(int player_id);
            void handle_player_join(const PlayerJoinEvent& e);
            void handle_player_leave(const PlayerLeaveEvent& e);
            void handle_player_move(const PlayerMoveEvent& e);
            void handle_entity_create(const EntityCreateEvent& e);
            void handle_entity_destroy(const EntityDestroyEvent& e);
            void send_position_updates(registry& ecs_registry);

            // Helper methods for remote player management
            entity create_remote_player(registry& ecs_registry, int player_id, float x, float y);
            void update_remote_player_position(registry& ecs_registry, int player_id, float x, float y);
};