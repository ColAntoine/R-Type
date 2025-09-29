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

    setup_entities();

    // Set up network message callback
    client->on_message_received = [this](const std::string& message) {
        this->handle_network_message(message);
    };

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

    // Create some static entities for testing
    entity wall1 = ecs_registry.spawn_entity();
    ecs_registry.add_component(wall1, position(300.0f, 200.0f));
    ecs_registry.add_component(wall1, drawable(100.0f, 50.0f, 128, 128, 128, 255)); // Gray wall
    ecs_registry.add_component(wall1, collider(100.0f, 50.0f));

    entity wall2 = ecs_registry.spawn_entity();
    ecs_registry.add_component(wall2, position(500.0f, 400.0f));
    ecs_registry.add_component(wall2, drawable(80.0f, 80.0f, 0, 255, 0, 255)); // Green wall
    ecs_registry.add_component(wall2, collider(80.0f, 80.0f));
}

void GameEngine::update_systems(float dt) {
    // Run ECS systems in order
    control_system(ecs_registry);           // Handle player input
    position_system(ecs_registry, dt);      // Update positions based on velocity
    collision_system(ecs_registry);         // Handle collisions
}

void GameEngine::render_ui() {
    // Draw UI elements
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
    static int frame_counter = 0;
    if (++frame_counter >= 5) {
        auto *pos_arr = ecs_registry.get_if<position>();

        if (pos_arr && pos_arr->size() > static_cast<size_t>(player_entity)) {
            auto& player_pos = (*pos_arr)[static_cast<size_t>(player_entity)];

            Protocol::PositionUpdate pos_update;
            pos_update.player_id = local_player_id;
            pos_update.position_x = player_pos.x;
            pos_update.position_y = player_pos.y;

            uint8_t buffer[sizeof(Protocol::PacketHeader) + sizeof(Protocol::PositionUpdate)];
            size_t packet_size = Protocol::create_packet(buffer,
                                                        static_cast<uint8_t>(Protocol::RTypeMessage::POSITION_UPDATE),
                                                        &pos_update, sizeof(pos_update));

            client->send_message(std::string(reinterpret_cast<const char*>(buffer), packet_size));
        }
        frame_counter = 0;
    }
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
        window->begin_drawing();
        window->clear_background(DARKBLUE);
        draw_system(ecs_registry);
        render_ui();
        window->end_drawing();
    }
    std::cout << "Game engine stopped" << std::endl;
}

void GameEngine::handle_network_message(const std::string& message) {
    Protocol::PacketHeader header;
    const uint8_t* payload;

    if (Protocol::parse_packet(reinterpret_cast<const uint8_t*>(message.c_str()), message.length(), header, payload)) {
        handle_binary_message(header, payload);
    }
}

void GameEngine::handle_binary_message(const Protocol::PacketHeader& header, const uint8_t* payload) {
    if (header.message_type == static_cast<uint8_t>(Protocol::SystemMessage::SERVER_ACCEPT)) {
        if (header.payload_size >= sizeof(Protocol::ServerAccept)) {
            const Protocol::ServerAccept* accept = reinterpret_cast<const Protocol::ServerAccept*>(payload);
            local_player_id = accept->player_id;
            std::cout << "Assigned Player ID: " << local_player_id << std::endl;
        }
    }
    else if (header.message_type == static_cast<uint8_t>(Protocol::SystemMessage::PLAYER_DISCONNECT)) {
        if (header.payload_size >= sizeof(Protocol::PlayerDisconnect)) {
            const Protocol::PlayerDisconnect* disconnect = reinterpret_cast<const Protocol::PlayerDisconnect*>(payload);
            remove_remote_player(disconnect->player_id);
        }
    }
    else if (header.message_type == static_cast<uint8_t>(Protocol::RTypeMessage::ENTITY_UPDATE)) {
        if (header.payload_size >= sizeof(Protocol::EntityUpdate)) {
            const Protocol::EntityUpdate* update = reinterpret_cast<const Protocol::EntityUpdate*>(payload);

            if (static_cast<int>(update->entity_id) != local_player_id) {
                update_remote_player(update->entity_id, update->position_x, update->position_y);
            }
        }
    }
}

void GameEngine::update_remote_player(int player_id, float x, float y) {
    auto it = remote_players.find(player_id);
    if (it != remote_players.end()) {
        // Update existing remote player position
        auto *pos_arr = ecs_registry.get_if<position>();
        if (pos_arr && pos_arr->size() > static_cast<size_t>(it->second)) {
            (*pos_arr)[static_cast<size_t>(it->second)] = position(x, y);
        }
    } else {
        // Create new remote player
        remote_players[player_id] = create_remote_player(player_id, x, y);
    }
}

entity GameEngine::create_remote_player(int player_id, float x, float y) {
    entity remote = ecs_registry.spawn_entity();
    ecs_registry.add_component(remote, position(x, y));
    ecs_registry.add_component(remote, collider(40.0f, 40.0f));
    ecs_registry.add_component(remote, drawable(40.0f, 40.0f, 128, 0, 128, 255)); // Purple for remote players
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