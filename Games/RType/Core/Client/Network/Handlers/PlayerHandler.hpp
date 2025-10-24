#pragma once

#include <cstddef>
#include <optional>
#include "ECS/Registry.hpp"
#include "ECS/DLLoader.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Animation.hpp"
#include "ECS/Components/Velocity.hpp"
#include "Entity/Components/Controllable/Controllable.hpp"

class PlayerHandler {
    public:
        PlayerHandler(registry& registry, DLLoader& loader);
        void on_player_join(const char* payload, size_t size);
        void on_player_quit(const char* payload, size_t size);
        void on_entity_create(const char* payload, size_t size);
        void on_player_spawn(const char* payload, size_t size);
        void on_player_remote_spawn(const char* payload, size_t size);

    private:
        registry& registry_;
        DLLoader& loader_;
        // Map remote player IDs (from server) to local entity IDs so we can remove them later
        std::unordered_map<uint32_t, entity> remote_player_map_;
        // local player entity id if assigned
        std::optional<entity> local_player_ent_;
};
