#include "PlayerHandler.hpp"
#include <iostream>
#include <cstring>
#include "Core/Server/Protocol/Protocol.hpp"

PlayerHandler::PlayerHandler(registry& registry, DLLoader& loader)
    : registry_(registry), loader_(loader)
{
}

void PlayerHandler::on_player_join(const char* payload, size_t size) {
    using RType::Protocol::PlayerInfo;
    if (!payload || size < sizeof(PlayerInfo)) {
        std::cerr << "[PlayerMsg] Invalid PLAYER_JOIN payload" << std::endl;
        return;
    }

    const PlayerInfo* pi = reinterpret_cast<const PlayerInfo*>(payload);
    std::string name(pi->name, strnlen(pi->name, 32));
    std::cout << "[PlayerMsg] Player joined: id=" << pi->player_id << " name=" << name << std::endl;

    // Create a local representation: spawn an entity with position only.
    auto ent = registry_.spawn_entity();
    // remember mapping from remote player id -> local entity
    remote_player_map_[pi->player_id] = ent;
    std::cout << "player_id" << pi->player_id << std::endl;
    float spawn_x = 100.0f;
    float spawn_y = 100.0f;

    if (&loader_) {
        auto factory = loader_.get_factory();
        if (factory) {
            // Try to use position component factory if available
            factory->create_component<position>(registry_, ent, spawn_x, spawn_y);
            factory->create_component<animation>(registry_, ent, "Games/RType/Assets/dedsec_eyeball-Sheet.png", 400.0f, 400.0f, 0.25f, 0.25f, 0, true);
            return;
        }
    }

    registry_.emplace_component<position>(ent, spawn_x, spawn_y);
}

void PlayerHandler::on_entity_create(const char* payload, size_t size) {
    using RType::Protocol::EntityCreate;
    if (!payload || size < sizeof(EntityCreate)) {
        std::cerr << "[PlayerMsg] Invalid ENTITY_CREATE payload" << std::endl;
        return;
    }
    EntityCreate ec;
    memcpy(&ec, payload, sizeof(ec));

    // spawn a local entity and attach components according to server description
    auto ent = registry_.spawn_entity();
    // If the entity corresponds to a remote player token, map it
    uint32_t server_ent_id = ec.entity_id;
    remote_player_map_[server_ent_id] = ent;

    float x = ec.x;
    float y = ec.y;
    if (&loader_) {
        auto factory = loader_.get_factory();
        if (factory) {
            factory->create_component<position>(registry_, ent, x, y);
            factory->create_component<velocity>(registry_, ent, 0.0f, 0.0f);
            factory->create_component<controllable>(registry_, ent, 100.0f);
            factory->create_component<animation>(registry_, ent, "Games/RType/Assets/dedsec_eyeball-Sheet.png", 400.0f, 400.0f, 0.25f, 0.25f, 0, true);
            return;
        }
    }
    registry_.emplace_component<position>(ent, x, y);
}

void PlayerHandler::on_player_spawn(const char* payload, size_t size) {
    using RType::Protocol::PlayerSpawn;
    if (!payload || size < sizeof(PlayerSpawn)) {
        std::cerr << "[PlayerMsg] Invalid PLAYER_SPAWN payload" << std::endl;
        return;
    }
    PlayerSpawn ps;
    memcpy(&ps, payload, sizeof(ps));

    std::cout << "[PlayerMsg] Received PLAYER_SPAWN token=" << ps.player_token << " server_ent=" << ps.server_entity << std::endl;

    // spawn a local entity and attach components according to server description
    auto ent = registry_.spawn_entity();
    remote_player_map_[ps.player_token] = ent;
    local_player_ent_ = ent;

    float x = ps.x;
    float y = ps.y;
    if (&loader_) {
        auto factory = loader_.get_factory();
        if (factory) {
            factory->create_component<position>(registry_, ent, x, y);
            factory->create_component<velocity>(registry_, ent, 0.0f, 0.0f);
            factory->create_component<controllable>(registry_, ent, 100.0f);
            factory->create_component<animation>(registry_, ent, "Games/RType/Assets/dedsec_eyeball-Sheet.png", 400.0f, 400.0f, 0.25f, 0.25f, 0, true);
            return;
        }
    }
    registry_.emplace_component<position>(ent, x, y);
}

void PlayerHandler::on_player_remote_spawn(const char* payload, size_t size) {
    using RType::Protocol::PlayerRemoteSpawn;
    if (!payload || size < sizeof(PlayerRemoteSpawn)) {
        std::cerr << "[PlayerMsg] Invalid PLAYER_SPAWN_REMOTE payload" << std::endl;
        return;
    }
    PlayerRemoteSpawn ps;
    memcpy(&ps, payload, sizeof(ps));

    std::cout << "[PlayerMsg] Received PLAYER_SPAWN_REMOTE token=" << ps.player_token << " server_ent=" << ps.server_entity << std::endl;

    // Spawn a local representation for a remote player
    auto ent = registry_.spawn_entity();
    remote_player_map_[ps.player_token] = ent;

    float x = ps.x;
    float y = ps.y;
    if (&loader_) {
        auto factory = loader_.get_factory();
        if (factory) {
            factory->create_component<position>(registry_, ent, x, y);
            factory->create_component<velocity>(registry_, ent, 0.0f, 0.0f);
            factory->create_component<animation>(registry_, ent, "Games/RType/Assets/dedsec_eyeball-Sheet.png", 400.0f, 400.0f, 0.25f, 0.25f, 0, true);
            return;
        }
    }
    registry_.emplace_component<position>(ent, x, y);
}


void PlayerHandler::on_player_quit(const char* payload, size_t size) {
    using RType::Protocol::PlayerInfo;
    // The server sends a small struct with the player_id to remove; we'll accept PlayerInfo or direct uint32
    uint32_t pid = 0;
    if (payload && size >= sizeof(uint32_t)) {
        // try PlayerInfo first
        if (size >= sizeof(PlayerInfo)) {
            const PlayerInfo* pi = reinterpret_cast<const PlayerInfo*>(payload);
            pid = pi->player_id;
        } else {
            memcpy(&pid, payload, sizeof(uint32_t));
        }
    } else {
        std::cerr << "[PlayerMsg] Invalid PLAYER_LEAVE payload" << std::endl;
        return;
    }

    auto it = remote_player_map_.find(pid);
    if (it == remote_player_map_.end()) {
        std::cout << "[PlayerMsg] PLAYER_LEAVE for unknown player id=" << pid << std::endl;
        return;
    }

    entity ent = it->second;
    std::cout << "[PlayerMsg] Removing remote player entity for player_id=" << pid << " ent=" << static_cast<size_t>(ent) << std::endl;
    registry_.kill_entity(ent);
    remote_player_map_.erase(it);
}
