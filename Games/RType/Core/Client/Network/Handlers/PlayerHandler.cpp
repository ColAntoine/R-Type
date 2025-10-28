#include "PlayerHandler.hpp"
#include <iostream>
#include <cstring>
#include <algorithm>
#include "Core/Server/Protocol/Protocol.hpp"
#include "Entity/Components/Weapon/Weapon.hpp"
#include "Entity/Components/Score/Score.hpp"
#include "Entity/Components/Health/Health.hpp"
#include "Entity/Components/Player/Player.hpp"
#include "ECS/Components/Collider.hpp"

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
    auto factory = loader_.get_factory();
    if (factory) {
        factory->create_component<position>(registry_, ent, x, y);
        factory->create_component<velocity>(registry_, ent, 0.0f, 0.0f);
        factory->create_component<controllable>(registry_, ent, 100.0f);
        factory->create_component<animation>(registry_, ent, "Games/RType/Assets/dedsec_eyeball-Sheet.png", 400.0f, 400.0f, 0.25f, 0.25f, 0, true);
        return;
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
    auto factory = loader_.get_factory();
    if (factory) {
        // Create full player with all components (matching InGameState::createPlayer)
        factory->create_component<position>(registry_, ent, x, y);
        factory->create_component<velocity>(registry_, ent, 0.0f, 0.0f);
        factory->create_component<animation>(registry_, ent, "Games/RType/Assets/dedsec_eyeball-Sheet.png", 400.0f, 400.0f, 0.25f, 0.25f, 0, true);
        factory->create_component<controllable>(registry_, ent, 300.0f);
        factory->create_component<Weapon>(registry_, ent);
        factory->create_component<collider>(registry_, ent, 100.f, 100.f, -50.f, -50.f);
        factory->create_component<Score>(registry_, ent);
        factory->create_component<Health>(registry_, ent);
        factory->create_component<Player>(registry_, ent);
        std::cout << "[PlayerMsg] Created full player entity with all components" << std::endl;
        return;
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
    auto factory = loader_.get_factory();
    if (factory) {
        factory->create_component<position>(registry_, ent, x, y);
        factory->create_component<velocity>(registry_, ent, 0.0f, 0.0f);
        factory->create_component<animation>(registry_, ent, "Games/RType/Assets/dedsec_eyeball-Sheet.png", 400.0f, 400.0f, 0.25f, 0.25f, 0, true);
        return;
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
    } else {
        entity ent = it->second;
        registry_.kill_entity(ent);
        remote_player_map_.erase(it);
    }

    // Remove player from cached player list
    auto player_it = std::find_if(last_player_list_.begin(), last_player_list_.end(),
        [pid](const RType::Protocol::PlayerInfo& p) { return p.player_id == pid; });

    if (player_it != last_player_list_.end()) {
        last_player_list_.erase(player_it);
        if (client_list_callback_)
            client_list_callback_(last_player_list_);
    }
}

void PlayerHandler::on_client_list(const char* payload, size_t size) {
    using RType::Protocol::ClientListUpdate;
    using RType::Protocol::PlayerInfo;

    if (!payload || size < sizeof(ClientListUpdate)) {
        std::cerr << "[PlayerMsg] Invalid CLIENT_LIST payload" << std::endl;
        return;
    }

    ClientListUpdate clu;
    memcpy(&clu, payload, sizeof(ClientListUpdate));

    std::vector<PlayerInfo> players;
    for (uint8_t i = 0; i < clu.player_count && i < 8; ++i) {
        const PlayerInfo &pi = clu.players[i];
        players.push_back(pi);
    }

    // Cache the player list
    last_player_list_ = players;

    // Invoke callback if registered
    if (client_list_callback_) {
        client_list_callback_(players);
    }
}

void PlayerHandler::set_client_list_callback(ClientListCallback callback) {
    client_list_callback_ = callback;

    // Immediately invoke with cached data if available
    if (callback && !last_player_list_.empty()) {
        callback(last_player_list_);
    }
}

void PlayerHandler::set_game_start_callback(GameStartCallback callback) {
    game_start_callback_ = callback;
}

void PlayerHandler::on_game_start(const char* payload, size_t size) {
    (void)payload; // unused
    (void)size;    // unused

    // Invoke callback if registered
    if (game_start_callback_) {
        std::cout << "[PlayerHandler] Invoking game start callback" << std::endl;
        game_start_callback_();
    }
}
