#include "game_engine.hpp"
#include "ecs/systems.hpp"
#include <iostream>
#include <sstream>
#include <chrono>
#include <algorithm>

GameEngine::GameEngine() : window(nullptr), client(nullptr), player_entity(0), initialized(false), local_player_id(0) {
}

GameEngine::~GameEngine() {
    shutdown();
}

bool GameEngine::initialize(GameWindow* game_window, UDPClient* network_client) {
    if (initialized) {
        std::cout << "Game engine already initialized" << std::endl;
        return true;
    }

    window = game_window;
    client = network_client;

    if (!window || !client) {
        std::cerr << "Invalid window or client provided to game engine" << std::endl;
        return false;
    }

    // Register ECS component types
    ecs_registry.register_component<position>();
    ecs_registry.register_component<velocity>();
    ecs_registry.register_component<drawable>();
    ecs_registry.register_component<controllable>();
    ecs_registry.register_component<collider>();
    ecs_registry.register_component<remote_player>();
    ecs_registry.register_component<enemy>();
    ecs_registry.register_component<lifetime>();
    ecs_registry.register_component<spawner>();

    setup_entities();

    // Set up network message callback
    client->on_message_received = [this](const std::string& message) {
        this->handle_network_message(message);
    };

    // Send connection request to server
    send_connection_request();

    initialized = true;
    std::cout << "Game engine initialized successfully" << std::endl;
    return true;
}

void GameEngine::setup_entities() {
    // Create player entity
    player_entity = ecs_registry.spawn_entity();
    ecs_registry.add_component(player_entity, position(100.0f, 300.0f));
    ecs_registry.add_component(player_entity, velocity(0.0f, 0.0f));
    ecs_registry.add_component(player_entity, drawable(40.0f, 40.0f, 255, 0, 0, 255)); // Red player
    ecs_registry.add_component(player_entity, controllable(200.0f)); // Speed of 200
    ecs_registry.add_component(player_entity, collider(40.0f, 40.0f));

}

void GameEngine::interpolate_remote_players(float dt) {
    const float MAX_EXTRAP_MS = 150.0f;
    const float CORRECTION_DURATION_MS = 150.0f;
    const float SNAP_THRESHOLD_PX = 30.0f;

    auto now_ms = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());

    for (auto &kv : remote_states) {
        int player_id = kv.first;
        RemoteState &s = kv.second;

        int64_t server_dt_ms = static_cast<int64_t>(now_ms) - static_cast<int64_t>(s.timestamp);
        if (server_dt_ms < 0) server_dt_ms = 0;
        if (server_dt_ms > static_cast<int64_t>(MAX_EXTRAP_MS)) server_dt_ms = static_cast<int64_t>(MAX_EXTRAP_MS);
        float proj_seconds = static_cast<float>(server_dt_ms) / 1000.0f;
        float projected_x = s.x + s.vx * proj_seconds;
        float projected_y = s.y + s.vy * proj_seconds;

        if (s.correction_duration_ms <= 0.0f) {
            s.correction_duration_ms = CORRECTION_DURATION_MS;
            s.correction_time_remaining_ms = 0.0f;
            s.target_x = projected_x;
            s.target_y = projected_y;
        }

        float corr_x = projected_x - s.displayed_x;
        float corr_y = projected_y - s.displayed_y;
        float corr_dist_sq = corr_x * corr_x + corr_y * corr_y;

        if (corr_dist_sq > (SNAP_THRESHOLD_PX * SNAP_THRESHOLD_PX)) {
            s.displayed_x = projected_x;
            s.displayed_y = projected_y;
            s.correction_time_remaining_ms = 0.0f;
            s.target_x = projected_x;
            s.target_y = projected_y;
        } else {
            if (std::abs(s.target_x - projected_x) > 0.01f || std::abs(s.target_y - projected_y) > 0.01f) {
                s.target_x = projected_x;
                s.target_y = projected_y;
                s.correction_time_remaining_ms = s.correction_duration_ms;
            }

            if (s.correction_time_remaining_ms > 0.0f) {
                float step_ms = dt * 1000.0f;
                float t = std::min(step_ms / s.correction_time_remaining_ms, 1.0f);
                s.displayed_x += (s.target_x - s.displayed_x) * t;
                s.displayed_y += (s.target_y - s.displayed_y) * t;
                s.correction_time_remaining_ms -= step_ms;
                if (s.correction_time_remaining_ms < 0.0f) s.correction_time_remaining_ms = 0.0f;
            } else {
                s.displayed_x = s.target_x;
                s.displayed_y = s.target_y;
            }
        }

        auto it = remote_players.find(player_id);
        if (it != remote_players.end()) {
            auto *pos_arr = ecs_registry.get_if<position>();
            if (pos_arr && pos_arr->size() > static_cast<size_t>(it->second)) {
                (*pos_arr)[static_cast<size_t>(it->second)] = position(s.displayed_x, s.displayed_y);
            }
        }
    }
}

void GameEngine::update_systems(float dt) {
    // Run ECS systems in order
    control_system(ecs_registry);                                    // Handle player input
    position_system(ecs_registry, dt);                               // Update positions based on velocity
    collision_system(ecs_registry);                                  // Handle collisions
}

void GameEngine::render_ui() {
    window->draw_text("R-Type Client - Multiplayer", 10, 10, 20, WHITE);

    std::ostringstream status;
    status << "FPS: " << window->get_fps() << " | Player " << local_player_id << " | Players: " << (remote_players.size() + 1);
    if (auto *pos_arr = ecs_registry.get_if<position>(); pos_arr && pos_arr->size() > static_cast<size_t>(player_entity)) {
        auto& player_pos = (*pos_arr)[static_cast<size_t>(player_entity)];
        status << " | You: (" << (int)player_pos.x << ", " << (int)player_pos.y << ")";
    }
    window->draw_text(status.str(), 10, 40, 16, LIGHTGRAY);

    window->draw_text("Red = You, Purple = Other players", 10, 65, 14, LIGHTGRAY);
    window->draw_text("Controls: Arrow Keys to move, ESC to quit", 10, window->get_height() - 30, 16, LIGHTGRAY);
}

void GameEngine::send_player_position() {
    if (local_player_id <= 0) {
        return;
    }

    static int frame_counter = 0;
    if (++frame_counter >= 5) {
        auto *pos_arr = ecs_registry.get_if<position>();

        if (pos_arr && pos_arr->size() > static_cast<size_t>(player_entity)) {
            auto& player_pos = (*pos_arr)[static_cast<size_t>(player_entity)];

            // Compute velocity based on last sent position/time to help remote interpolation
            static float last_x = player_pos.x;
            static float last_y = player_pos.y;
            static uint32_t last_time_ms = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());

            uint32_t now_ms = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());

            float vx = 0.0f;
            float vy = 0.0f;
            uint32_t dt_ms = now_ms - last_time_ms;
            if (dt_ms > 0) {
                float dt_seconds = static_cast<float>(dt_ms) / 1000.0f;
                vx = (player_pos.x - last_x) / dt_seconds;
                vy = (player_pos.y - last_y) / dt_seconds;
            }

            RType::Protocol::PositionUpdate pos_update;
            pos_update.entity_id = local_player_id;
            pos_update.x = player_pos.x;
            pos_update.y = player_pos.y;
            pos_update.vx = vx;
            pos_update.vy = vy;
            pos_update.timestamp = now_ms;

            last_x = player_pos.x;
            last_y = player_pos.y;
            last_time_ms = now_ms;

            auto packet = RType::Protocol::create_packet(
                static_cast<uint8_t>(RType::Protocol::GameMessage::POSITION_UPDATE),
                pos_update
            );

            client->send_message(std::string(reinterpret_cast<const char*>(packet.data()), packet.size()));
        }
        frame_counter = 0;
    }
}

void GameEngine::send_connection_request() {
    RType::Protocol::ClientConnect connect_msg;
    strncpy(connect_msg.player_name, "Player", sizeof(connect_msg.player_name) - 1);
    connect_msg.player_name[sizeof(connect_msg.player_name) - 1] = '\0';
    connect_msg.client_version = 1;

    auto packet = RType::Protocol::create_packet(
        static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_CONNECT),
        connect_msg
    );

    client->send_message(std::string(reinterpret_cast<const char*>(packet.data()), packet.size()));
    std::cout << "Sent connection request to server" << std::endl;
}

void GameEngine::run() {
    if (!initialized) {
        std::cerr << "Game engine not initialized" << std::endl;
        return;
    }
    std::cout << "Game engine started. Use Arrow Keys to move, ESC to quit." << std::endl;
    while (client->is_connected() && !window->should_window_close()) {
        float dt = window->get_frame_time();
        if (window->is_key_pressed(KEY_ESCAPE)) {
            std::cout << "ESC pressed - closing game" << std::endl;
            break;
        }
        update_systems(dt);
        send_player_position();

        // Interpolate remote players for smooth rendering
        interpolate_remote_players(dt);

        window->begin_drawing();
        window->clear_background(DARKBLUE);
        draw_system(ecs_registry);
        render_ui();
        window->end_drawing();
    }
    std::cout << "Game engine stopped" << std::endl;
}

void GameEngine::handle_network_message(const std::string& message) {
    RType::Protocol::PacketParser parser(message.c_str(), message.size());

    if (parser.is_valid()) {
        handle_binary_message(parser.get_header(),
                              reinterpret_cast<const uint8_t*>(parser.get_payload()));
    }
}

void GameEngine::handle_binary_message(const RType::Protocol::PacketHeader& header, const uint8_t* payload) {
    if (header.message_type == static_cast<uint8_t>(RType::Protocol::SystemMessage::SERVER_ACCEPT)) {
        if (header.payload_size >= sizeof(RType::Protocol::ServerAccept)) {
            const RType::Protocol::ServerAccept* accept = reinterpret_cast<const RType::Protocol::ServerAccept*>(payload);
            local_player_id = accept->player_id;
            std::cout << "Server accepted connection. Player ID: " << local_player_id << std::endl;
        }
    }
    else if (header.message_type == static_cast<uint8_t>(RType::Protocol::GameMessage::POSITION_UPDATE)) {
        if (header.payload_size >= sizeof(RType::Protocol::PositionUpdate)) {
            const RType::Protocol::PositionUpdate* pos_update = reinterpret_cast<const RType::Protocol::PositionUpdate*>(payload);
            // Only handle position updates from OTHER players, not our own
            if (pos_update->entity_id != local_player_id) {
                update_remote_player(pos_update->entity_id, pos_update->x, pos_update->y, pos_update->vx, pos_update->vy, pos_update->timestamp);
            }
        }
    }
    else if (header.message_type == static_cast<uint8_t>(RType::Protocol::GameMessage::ENTITY_CREATE)) {
        if (header.payload_size >= sizeof(RType::Protocol::EntityCreate)) {
            const RType::Protocol::EntityCreate* create = reinterpret_cast<const RType::Protocol::EntityCreate*>(payload);
            std::cout << "Entity created: " << create->entity_id << std::endl;
        }
    }
    else if (header.message_type == static_cast<uint8_t>(RType::Protocol::GameMessage::ENTITY_DESTROY)) {
        if (header.payload_size >= sizeof(RType::Protocol::EntityDestroy)) {
            const RType::Protocol::EntityDestroy* destroy = reinterpret_cast<const RType::Protocol::EntityDestroy*>(payload);
            std::cout << "Entity destroyed: " << destroy->entity_id << std::endl;
        }
    }
}

void GameEngine::update_remote_player(int player_id, float x, float y, float vx, float vy, uint32_t timestamp) {
    auto it = remote_players.find(player_id);

    // Update remote state (store latest authoritative state)
    RemoteState s;
    s.x = x;
    s.y = y;
    s.vx = vx;
    s.vy = vy;
    s.timestamp = timestamp;
    // Keep displayed position where it was if we already had a state, otherwise initialize
    auto existing = remote_states.find(player_id);
    if (existing != remote_states.end()) {
        s.displayed_x = existing->second.displayed_x;
        s.displayed_y = existing->second.displayed_y;
    } else {
        s.displayed_x = x;
        s.displayed_y = y;
    }
    // init correction targets
    s.target_x = x;
    s.target_y = y;
    s.correction_duration_ms = 0.0f; // will be set on next interpolation pass
    s.correction_time_remaining_ms = 0.0f;
    remote_states[player_id] = s;

    if (it != remote_players.end()) {
        // existing remote entity - position will be updated by interpolation step
    } else {
        // Create new remote player
        remote_players[player_id] = create_remote_player(player_id, x, y);
    }
}

entity GameEngine::create_remote_player(int player_id, float x, float y) {
    entity remote = ecs_registry.spawn_entity();
    ecs_registry.add_component(remote, position(x, y));
    ecs_registry.add_component(remote, collider(40.0f, 40.0f));
    ecs_registry.add_component(remote, drawable(40.0f, 40.0f, 0, 100, 255, 255)); // Blue for remote players (same size as local player)
    ecs_registry.add_component(remote, remote_player(std::to_string(player_id)));
    // Note: Remote players don't have controllable or velocity components
    return remote;
}

void GameEngine::remove_remote_player(int player_id) {
    auto it = remote_players.find(player_id);
    if (it != remote_players.end()) {
        ecs_registry.kill_entity(it->second);
        remote_players.erase(it);
    }
}

void GameEngine::update_enemy(uint32_t enemy_id, float x, float y, float vx, float vy) {
    auto it = enemies.find(enemy_id);
    if (it != enemies.end()) {
        // Update existing enemy position and velocity
        auto *pos_arr = ecs_registry.get_if<position>();
        auto *vel_arr = ecs_registry.get_if<velocity>();
        if (pos_arr && pos_arr->size() > static_cast<size_t>(it->second)) {
            (*pos_arr)[static_cast<size_t>(it->second)] = position(x, y);
        }
        if (vel_arr && vel_arr->size() > static_cast<size_t>(it->second)) {
            (*vel_arr)[static_cast<size_t>(it->second)] = velocity(vx, vy);
        }
    } else {
        // Create new enemy
        enemies[enemy_id] = create_enemy(enemy_id, x, y);
    }
}

entity GameEngine::create_enemy(uint32_t enemy_id, float x, float y) {
    entity enemy_entity = ecs_registry.spawn_entity();
    ecs_registry.add_component(enemy_entity, position(x, y));
    ecs_registry.add_component(enemy_entity, velocity(-100.0f, 0.0f)); // Default left movement
    ecs_registry.add_component(enemy_entity, collider(30.0f, 30.0f));
    ecs_registry.add_component(enemy_entity, drawable(30.0f, 30.0f, 255, 165, 0, 255)); // Orange enemy
    ecs_registry.add_component(enemy_entity, enemy(0, 1.0f)); // Type 0, health 1
    // Note: Enemies don't have controllable component - they're not player controlled
    return enemy_entity;
}

void GameEngine::shutdown() {
    if (initialized) {
        // Clear remote players
        remote_players.clear();
        initialized = false;
        std::cout << "Game engine shut down" << std::endl;
    }
}

bool GameEngine::is_running() const {
    return initialized && client && client->is_connected() && window && !window->should_window_close();
}