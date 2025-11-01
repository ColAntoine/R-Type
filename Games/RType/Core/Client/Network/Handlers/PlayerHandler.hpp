#pragma once

#include <cstddef>
#include <optional>
#include <functional>
#include <vector>
#include "ECS/Registry.hpp"
#include "ECS/ILoader.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Animation.hpp"
#include "ECS/Components/Velocity.hpp"
#include "Entity/Components/Controllable/Controllable.hpp"
#include "Core/Server/Protocol/Protocol.hpp"
#include <unordered_map>

class PlayerHandler {
    public:
        using ClientListCallback = std::function<void(const std::vector<RType::Protocol::PlayerInfo>&)>;
        using GameStartCallback = std::function<void()>;

        PlayerHandler(registry& registry, ILoader& loader);
        void on_player_join(const char* payload, size_t size);
        void on_client_list(const char* payload, size_t size);
        void on_player_quit(const char* payload, size_t size);
        void on_entity_create(const char* payload, size_t size);
        void on_player_spawn(const char* payload, size_t size);
        void on_player_remote_spawn(const char* payload, size_t size);
        void on_game_start(const char* payload, size_t size);
        void on_position_update(const char* payload, size_t size);
        void on_player_shoot(const char* payload, size_t size);
        void on_player_unshoot(const char* payload, size_t size);
        void update();

        // Register a callback for when CLIENT_LIST is received
        void set_client_list_callback(ClientListCallback callback);

        // Register a callback for when START_GAME is received
        void set_game_start_callback(GameStartCallback callback);

    // Register a callback for when the server informs about an instance created (port)
    void set_instance_created_callback(std::function<void(uint16_t)> cb);

    // Called by network dispatcher when INSTANCE_CREATED arrives
    void on_instance_created(const char* payload, size_t size);
    // Instance list handling
    using InstanceListCallback = std::function<void(const std::vector<RType::Protocol::InstanceInfo>&)>;
    void set_instance_list_callback(InstanceListCallback cb);
    void on_instance_list(const char* payload, size_t size);

    private:
        registry& registry_;
        ILoader& loader_;
        // Map remote player IDs (from server) to local entity IDs so we can remove them later
        std::unordered_map<uint32_t, entity> remote_player_map_;
        // Tracks which remote players are currently holding the shoot input (reapplied every frame)
        std::unordered_map<uint32_t, bool> remote_is_shooting_;
        // local player entity id if assigned
        std::optional<entity> local_player_ent_;
        // Callback invoked when CLIENT_LIST is received
        ClientListCallback client_list_callback_;
        // Callback invoked when START_GAME is received
        GameStartCallback game_start_callback_;
        // Cache of the last received player list
        std::vector<RType::Protocol::PlayerInfo> last_player_list_;
    // Callback when an instance (lobby+game) is created by the server
    std::function<void(uint16_t)> instance_created_callback_;
    // Last known instance list
    std::vector<RType::Protocol::InstanceInfo> last_instance_list_;
    // Instance list callback
    InstanceListCallback instance_list_callback_;
};
