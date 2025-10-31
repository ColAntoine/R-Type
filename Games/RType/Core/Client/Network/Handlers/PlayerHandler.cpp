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
#include "Constants.hpp"

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

    uint32_t server_ent_id = ec.entity_id;

    // If we already have a mapping for this server entity id, update components instead
    auto it = remote_player_map_.find(server_ent_id);
    if (it != remote_player_map_.end()) {
        auto ent = it->second;
        float x = ec.x;
        float y = ec.y;
        auto factory = loader_.get_factory();
        auto* pos_arr = registry_.get_if<position>();
        if (factory) {
            if (!(pos_arr && pos_arr->has(ent))) factory->create_component<position>(registry_, ent, x, y);
            else { (*pos_arr)[ent] = position{x, y}; }
            auto* vel_arr = registry_.get_if<velocity>();
            if (!(vel_arr && vel_arr->has(ent))) factory->create_component<velocity>(registry_, ent, 0.0f, 0.0f);
            return;
        }
        if (!(pos_arr && pos_arr->has(ent))) {
            registry_.emplace_component<position>(ent, x, y);
        } else {
            (*pos_arr)[ent] = position{ec.x, ec.y};
        }
        return;
    }

    // Before spawning, check if a local Player entity already exists (created by InGame::createPlayer)
    // If so, map this server entity id to that existing entity and update its position/velocity
    auto *playerArr = registry_.get_if<Player>();
    if (playerArr && playerArr->size() > 0) {
        entity existing = entity(playerArr->entity_at(0));
        remote_player_map_[server_ent_id] = existing;
        float nx = ec.x;
        float ny = ec.y;
        auto* pos_arr = registry_.get_if<position>();
        if (pos_arr) {
            if (!pos_arr->has(existing)) registry_.emplace_component<position>(existing, nx, ny);
            else (*pos_arr)[existing] = position{nx, ny};
        } else {
            registry_.emplace_component<position>(existing, nx, ny);
        }
        auto* vel_arr = registry_.get_if<velocity>();
        if (vel_arr) {
            if (!vel_arr->has(existing)) registry_.emplace_component<velocity>(existing, 0.0f, 0.0f);
            else (*vel_arr)[existing] = velocity{0.0f, 0.0f};
        } else {
            registry_.emplace_component<velocity>(existing, 0.0f, 0.0f);
        }
        return;
    }

    // spawn a local entity and attach components according to server description
    auto ent = registry_.spawn_entity();
    // Map by server entity id so future updates can find it
    remote_player_map_[server_ent_id] = ent;

    float x = ec.x;
    float y = ec.y;
    auto factory = loader_.get_factory();
    if (factory) {
        // Only attach minimal components on generic ENTITY_CREATE. Full player components
        // (controllable, Weapon, Score, Health, Player, animation, collider) should be created on PLAYER_SPAWN.
        factory->create_component<position>(registry_, ent, x, y);
        factory->create_component<velocity>(registry_, ent, 0.0f, 0.0f);
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

    // Avoid duplicate spawn: if mapping already exists for this player token or server entity, reuse it
    auto it_token = remote_player_map_.find(ps.player_token);
    if (it_token != remote_player_map_.end()) {
        local_player_ent_ = it_token->second;
        std::cout << "[PlayerMsg] PLAYER_SPAWN: entity already exists locally, reusing" << std::endl;
        return;
    }
    auto it_server = remote_player_map_.find(ps.server_entity);
    if (it_server != remote_player_map_.end()) {
        local_player_ent_ = it_server->second;
        // ensure token mapping exists
        remote_player_map_[ps.player_token] = local_player_ent_.value();
        std::cout << "[PlayerMsg] PLAYER_SPAWN: found by server_entity, reusing and mapping token" << std::endl;
        return;
    }

    // spawn a local entity and attach components according to server description
    // Before creating a new entity, check if the registry already has a Player component
    // (created locally by InGame::createPlayer()). If so, reuse that entity to avoid
    // creating a duplicate when the client had pre-spawned its own player.
    auto *playerArr = registry_.get_if<Player>();
    if (playerArr && playerArr->size() > 0) {
        // reuse the first existing Player entity
        entity existing = entity(playerArr->entity_at(0));
        remote_player_map_[ps.player_token] = existing;
        remote_player_map_[ps.server_entity] = existing;
        local_player_ent_ = existing;
        std::cout << "[PlayerMsg] PLAYER_SPAWN: reusing existing local Player entity=" << static_cast<size_t>(existing) << std::endl;

        // Update position/velocity components if present or create them
        float nx = ps.x;
        float ny = ps.y;
        auto* pos_arr = registry_.get_if<position>();
        if (pos_arr) {
            if (!pos_arr->has(existing)) registry_.emplace_component<position>(existing, nx, ny);
            else (*pos_arr)[existing] = position{nx, ny};
        } else {
            registry_.emplace_component<position>(existing, nx, ny);
        }
        auto* vel_arr = registry_.get_if<velocity>();
        if (vel_arr) {
            if (!vel_arr->has(existing)) registry_.emplace_component<velocity>(existing, 0.0f, 0.0f);
            else (*vel_arr)[existing] = velocity{0.0f, 0.0f};
        } else {
            registry_.emplace_component<velocity>(existing, 0.0f, 0.0f);
        }

        return;
    }

    auto ent = registry_.spawn_entity();
    // map by both token and server entity id for consistency
    remote_player_map_[ps.player_token] = ent;
    remote_player_map_[ps.server_entity] = ent;
    local_player_ent_ = ent;

    float x = ps.x;
    float y = ps.y;
    auto factory = loader_.get_factory();
    if (factory) {
        // Create full player with all components (matching InGameState::createPlayer)
        factory->create_component<position>(registry_, ent, x, y);
        factory->create_component<velocity>(registry_, ent, 0.0f, 0.0f);
        factory->create_component<animation>(registry_, ent, std::string(RTYPE_PATH_ASSETS) + "dedsec_eyeball-Sheet.png", 400.0f, 400.0f, 0.25f, 0.25f, 0, true);
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

    // Avoid duplicate spawn: check existing mapping by token or server entity id
    if (remote_player_map_.find(ps.player_token) != remote_player_map_.end() || remote_player_map_.find(ps.server_entity) != remote_player_map_.end()) {
        std::cout << "[PlayerMsg] PLAYER_SPAWN_REMOTE: already have entity for this player, skipping spawn" << std::endl;
        // ensure both keys are mapped to the same entity
        if (remote_player_map_.find(ps.player_token) == remote_player_map_.end() && remote_player_map_.find(ps.server_entity) != remote_player_map_.end()) {
            remote_player_map_[ps.player_token] = remote_player_map_[ps.server_entity];
        }
        return;
    }

    // Spawn a local representation for a remote player
    auto ent = registry_.spawn_entity();
    // map by both token and server entity id
    remote_player_map_[ps.player_token] = ent;
    remote_player_map_[ps.server_entity] = ent;

    float x = ps.x;
    float y = ps.y;
    auto factory = loader_.get_factory();
    if (factory) {
        factory->create_component<position>(registry_, ent, x, y);
        factory->create_component<velocity>(registry_, ent, 0.0f, 0.0f);
        factory->create_component<animation>(registry_, ent, std::string(RTYPE_PATH_ASSETS) + "dedsec_eyeball-Sheet.png", 400.0f, 400.0f, 0.25f, 0.25f, 0, true);
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
        // remove any other mappings that point to the same entity (server_entity key etc.)
        std::vector<uint32_t> to_remove;
        for (const auto &kv : remote_player_map_) {
            if (kv.second == ent) to_remove.push_back(kv.first);
        }
        for (auto k : to_remove) remote_player_map_.erase(k);
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

void PlayerHandler::set_instance_created_callback(std::function<void(uint16_t)> cb) {
    instance_created_callback_ = std::move(cb);
}

void PlayerHandler::on_instance_created(const char* payload, size_t size) {
    if (!payload || size < sizeof(RType::Protocol::InstanceCreated)) return;
    RType::Protocol::InstanceCreated ic;
    memcpy(&ic, payload, sizeof(ic));
    uint16_t port = ic.port;
    if (instance_created_callback_) instance_created_callback_(port);
}

void PlayerHandler::set_instance_list_callback(InstanceListCallback cb) {
    instance_list_callback_ = std::move(cb);
    // If we already have a last known list, invoke immediately
    if (instance_list_callback_ && !last_instance_list_.empty()) instance_list_callback_(last_instance_list_);
}

void PlayerHandler::on_instance_list(const char* payload, size_t size) {
    if (!payload || size < sizeof(RType::Protocol::InstanceList)) return;
    RType::Protocol::InstanceList il;
    memcpy(&il, payload, sizeof(RType::Protocol::InstanceList));
    std::vector<RType::Protocol::InstanceInfo> list;
    for (uint8_t i = 0; i < il.instance_count && i < 8; ++i) {
        list.push_back(il.instances[i]);
    }
    last_instance_list_ = list;
    if (instance_list_callback_) instance_list_callback_(list);
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

void PlayerHandler::on_position_update(const char* payload, size_t size) {
    using RType::Protocol::PositionUpdate;

    if (!payload || size < sizeof(PositionUpdate)) {
        std::cerr << "[PlayerMsg] Invalid POSITION_UPDATE payload" << std::endl;
        return;
    }

    PositionUpdate pu;
    memcpy(&pu, payload, sizeof(pu));

    // Find the entity by player_token (entity_id is actually player_token)
    auto it = remote_player_map_.find(pu.entity_id);
    if (it == remote_player_map_.end()) {
        // This might be our own entity or an unknown player
        return;
    }

    entity ent = it->second;

    // Update position
    auto* pos_arr = registry_.get_if<position>();
    if (pos_arr && pos_arr->has(ent)) {
        position& pos = (*pos_arr)[ent];
        pos.x = pu.x;
        pos.y = pu.y;
    }

    // Update velocity
    auto* vel_arr = registry_.get_if<velocity>();
    if (vel_arr && vel_arr->has(ent)) {
        velocity& vel = (*vel_arr)[ent];
        vel.vx = pu.vx;
        vel.vy = pu.vy;
    }
}
