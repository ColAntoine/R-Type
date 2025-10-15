#include "Network.hpp"
#include "Core/PlatformMacros.hpp"
#include <iostream>

NetworkService::NetworkService(EventManager* event_manager) : event_manager_(event_manager) {
}

void NetworkService::initialize() {
    client_ = std::make_unique<UDPClient>();

    // Set up message handler
    client_->on_message_received = [this](const std::string& message) {
        handle_network_message(message);
    };
}

void NetworkService::shutdown() {
    if (client_ && connected_) {
        client_->disconnect();
    }
    connected_ = false;
}

void NetworkService::update(MAYBE_UNUSED float delta_time) {
    // Network service doesn't need regular updates
    // All communication is event-driven
}

bool NetworkService::connect_to_server(const std::string& ip, int port, const std::string& player_name) {
    server_ip_ = ip;
    server_port_ = port;
    player_name_ = player_name;
    connection_confirmed_.store(false);

    if (client_->connect_to_server(ip, port)) {
        client_->start_receiving();
        connected_ = true;

        // Send connection request
        send_connection_request();

        // Wait for server response with timeout (2 seconds)
        auto start_time = std::chrono::steady_clock::now();
        const auto timeout = std::chrono::seconds(2);

        while (std::chrono::steady_clock::now() - start_time < timeout) {
            if (connection_confirmed_.load()) {
                return true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // Timeout - no response from server
        disconnect();
        return false;
    }
    return false;
}

void NetworkService::disconnect() {
    if (client_ && connected_) {
        // Send proper disconnect message before disconnecting
        RType::Protocol::ClientDisconnect disconnect_msg;
        disconnect_msg.player_id = local_player_id_.load();
        disconnect_msg.reason = 0; // Voluntary disconnect

        auto packet = RType::Protocol::create_packet(
            static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_DISCONNECT),
            disconnect_msg
        );

        client_->send_data(reinterpret_cast<const char*>(packet.data()), packet.size());

        client_->disconnect();
        connected_ = false;
        event_manager_->emit(NetworkDisconnectedEvent());
    }
}

bool NetworkService::is_connected() const {
    return connected_ && client_ && client_->is_connected();
}

int NetworkService::get_local_player_id() const {
    return local_player_id_.load();
}

void NetworkService::send_player_position(float x, float y, float vx, float vy) {
    if (!is_connected() || local_player_id_ <= 0) return;

    RType::Protocol::PositionUpdate pos_update;
    pos_update.entity_id = local_player_id_;
    pos_update.x = x;
    pos_update.y = y;
    pos_update.vx = vx;
    pos_update.vy = vy;
    pos_update.timestamp = get_current_timestamp();

    auto packet = RType::Protocol::create_packet(
        static_cast<uint8_t>(RType::Protocol::GameMessage::POSITION_UPDATE),
        pos_update
    );

    client_->send_message(std::string(
        reinterpret_cast<const char*>(packet.data()), packet.size()));
}

void NetworkService::send_player_shoot(float start_x, float start_y, float dir_x, float dir_y, uint8_t weapon_type) {
    if (!is_connected() || local_player_id_ <= 0) return;

    RType::Protocol::PlayerShoot shoot_msg;
    shoot_msg.player_id = local_player_id_.load();
    shoot_msg.start_x = start_x;
    shoot_msg.start_y = start_y;
    shoot_msg.dir_x = dir_x;
    shoot_msg.dir_y = dir_y;
    shoot_msg.weapon_type = weapon_type;

    auto packet = RType::Protocol::create_packet(
        static_cast<uint8_t>(RType::Protocol::GameMessage::PLAYER_SHOOT),
        shoot_msg
    );

    client_->send_message(std::string(
        reinterpret_cast<const char*>(packet.data()), packet.size()));
}

void NetworkService::send_ready_signal(bool ready) {
    if (!is_connected() || local_player_id_ <= 0) {
        return;
    }

    RType::Protocol::ClientReady ready_msg;
    ready_msg.player_id = local_player_id_;
    ready_msg.ready_state = ready ? 1 : 0;

    auto packet = RType::Protocol::create_packet(
        static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_READY),
        ready_msg
    );

    client_->send_message(std::string(
        reinterpret_cast<const char*>(packet.data()), packet.size()));
}

void NetworkService::send_entity_destroy(uint32_t entity_network_id) {
    if (!is_connected() || local_player_id_ <= 0) return;

    RType::Protocol::EntityDestroy destroy_msg;
    destroy_msg.entity_id = entity_network_id;
    std::cout << "Sending ENTITY_DESTROY for entity ID " << entity_network_id << std::endl;
    auto packet = RType::Protocol::create_packet(
        static_cast<uint8_t>(RType::Protocol::GameMessage::ENTITY_DESTROY),
        destroy_msg
    );

    client_->send_message(std::string(
        reinterpret_cast<const char*>(packet.data()), packet.size()));
}

void NetworkService::send_connection_request() {
    RType::Protocol::ClientConnect connect_msg;

    // Ensure player name fits in the buffer (31 chars + null terminator)
    std::string safe_name = player_name_;
    if (safe_name.length() > 31) {
        safe_name = safe_name.substr(0, 31);
    }
    if (safe_name.empty()) {
        safe_name = "Player";
    }

    strncpy(connect_msg.player_name, safe_name.c_str(), sizeof(connect_msg.player_name) - 1);
    connect_msg.player_name[sizeof(connect_msg.player_name) - 1] = '\0';
    connect_msg.client_version = 1;

    auto packet = RType::Protocol::create_packet(
        static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_CONNECT),
        connect_msg
    );

    client_->send_message(std::string(
        reinterpret_cast<const char*>(packet.data()), packet.size()));
}

void NetworkService::handle_network_message(const std::string& message) {
    RType::Protocol::PacketParser parser(message.c_str(), message.size());

    if (!parser.is_valid()) return;

    auto header = parser.get_header();
    auto payload = reinterpret_cast<const uint8_t*>(parser.get_payload());

    if (header.message_type == static_cast<uint8_t>(RType::Protocol::SystemMessage::SERVER_ACCEPT)) {
        handle_server_accept(header, payload);
    } else if (header.message_type == static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_LIST)) {
        handle_client_list(header, payload);
    } else if (header.message_type == static_cast<uint8_t>(RType::Protocol::SystemMessage::START_GAME)) {
        handle_start_game(header, payload);
    } else if (header.message_type == static_cast<uint8_t>(RType::Protocol::GameMessage::POSITION_UPDATE)) {
        handle_position_update(header, payload);
    } else if (header.message_type == static_cast<uint8_t>(RType::Protocol::GameMessage::PLAYER_SHOOT)) {
        handle_player_shoot(header, payload);
    } else if (header.message_type == static_cast<uint8_t>(RType::Protocol::GameMessage::PLAYER_JOIN)) {
        handle_player_join(header, payload);
    } else if (header.message_type == static_cast<uint8_t>(RType::Protocol::GameMessage::PLAYER_LEAVE)) {
        handle_player_leave(header, payload);
    } else if (header.message_type == static_cast<uint8_t>(RType::Protocol::GameMessage::ENTITY_CREATE)) {
        handle_entity_create(header, payload);
    } else if (header.message_type == static_cast<uint8_t>(RType::Protocol::GameMessage::ENTITY_UPDATE)) {
        handle_entity_update(header, payload);
    } else if (header.message_type == static_cast<uint8_t>(RType::Protocol::GameMessage::ENTITY_DESTROY)) {
        handle_entity_destroy(header, payload);
    }
}

void NetworkService::handle_server_accept(const RType::Protocol::PacketHeader& header, const uint8_t* payload) {
    if (header.payload_size >= sizeof(RType::Protocol::ServerAccept)) {
        const auto* accept = reinterpret_cast<const RType::Protocol::ServerAccept*>(payload);
        local_player_id_ = accept->player_id;
        connection_confirmed_.store(true);  // Mark connection as confirmed
        event_manager_->emit(NetworkConnectedEvent(accept->player_id));
    }
}

void NetworkService::handle_client_list(const RType::Protocol::PacketHeader& header, const uint8_t* payload) {
    if (header.payload_size >= sizeof(RType::Protocol::ClientListUpdate)) {
        const auto* client_list = reinterpret_cast<const RType::Protocol::ClientListUpdate*>(payload);

        // Create PlayerList event with the received data
        std::vector<PlayerListEvent::PlayerInfo> players;
        for (int i = 0; i < client_list->player_count && i < 8; ++i) {
            const auto& server_player = client_list->players[i];
            PlayerListEvent::PlayerInfo player_info;
            player_info.player_id = server_player.player_id;
            player_info.is_ready = (server_player.ready_state == 1);
            player_info.name = std::string(server_player.name, 32);
            player_info.is_local_player = (server_player.player_id == local_player_id_.load());

            players.push_back(player_info);
        }

        // Emit event for UI update
        event_manager_->emit(PlayerListEvent(players));
    }
}

void NetworkService::handle_start_game(const RType::Protocol::PacketHeader& header, const uint8_t* payload) {
    if (header.payload_size >= sizeof(RType::Protocol::StartGame)) {
        const auto* start_game = reinterpret_cast<const RType::Protocol::StartGame*>(payload);

        // Emit start game event to trigger state transition
        event_manager_->emit(StartGameEvent(start_game->timestamp));
    }
}

void NetworkService::handle_position_update(const RType::Protocol::PacketHeader& header, const uint8_t* payload) {
    if (header.payload_size >= sizeof(RType::Protocol::PositionUpdate)) {
        const auto* pos_update = reinterpret_cast<const RType::Protocol::PositionUpdate*>(payload);

        // Only emit events for other players
        if (pos_update->entity_id != local_player_id_) {
            event_manager_->emit(PlayerMoveEvent(
                pos_update->entity_id,
                pos_update->x, pos_update->y,
                pos_update->vx, pos_update->vy
            ));
        }
    }
}

void NetworkService::handle_entity_create(const RType::Protocol::PacketHeader& header, const uint8_t* payload) {
    if (header.payload_size >= sizeof(RType::Protocol::EntityCreate)) {
        const auto* create = reinterpret_cast<const RType::Protocol::EntityCreate*>(payload);

        // Determine if this is an enemy based on entity_type or entity_id range
        // Enemy types: 1, 2, 3, 4
        if (create->entity_type >= 1 && create->entity_type <= 4) { // Enemy
            event_manager_->emit(EnemySpawnEvent(create->entity_id, create->entity_type,
                                               create->x, create->y, create->health));
        } else {
            // Generic entity creation
            event_manager_->emit(EntityCreateEvent(create->entity_id, create->entity_type,
                                                 create->x, create->y));
        }
    }
}

void NetworkService::handle_entity_update(const RType::Protocol::PacketHeader& header, const uint8_t* payload) {
    if (header.payload_size >= sizeof(RType::Protocol::PositionUpdate)) {
        const auto* update = reinterpret_cast<const RType::Protocol::PositionUpdate*>(payload);

        // Check if this is an enemy entity (you'd need to track entity types)
        // For now, assume entities with ID > 10000 are enemies
        if (update->entity_id > 10000) {
            event_manager_->emit(EnemyUpdateEvent(update->entity_id,
                                                update->x, update->y,
                                                update->vx, update->vy,
                                                100.0f)); // Default health since PositionUpdate doesn't have it
        } else if (update->entity_id != static_cast<uint32_t>(local_player_id_)) {
            // This is another player
            event_manager_->emit(PlayerMoveEvent(update->entity_id,
                                               update->x, update->y,
                                               update->vx, update->vy));
        }
    }
}

void NetworkService::handle_player_shoot(const RType::Protocol::PacketHeader& header, const uint8_t* payload) {
    if (header.payload_size >= sizeof(RType::Protocol::PlayerShoot)) {
        const auto* shoot = reinterpret_cast<const RType::Protocol::PlayerShoot*>(payload);
        if (static_cast<uint32_t>(local_player_id_.load()) == shoot->player_id) {
            return;
        }
        event_manager_->emit(PlayerShootEvent(static_cast<int>(shoot->player_id), shoot->start_x, shoot->start_y, shoot->dir_x, shoot->dir_y, shoot->weapon_type));
    }
}

void NetworkService::handle_entity_destroy(const RType::Protocol::PacketHeader& header, const uint8_t* payload) {
    if (header.payload_size >= sizeof(RType::Protocol::EntityDestroy)) {
        const auto* destroy = reinterpret_cast<const RType::Protocol::EntityDestroy*>(payload);

        // Check if this is an enemy entity
        if (destroy->entity_id > 10000) {
            event_manager_->emit(EnemyDestroyEvent(destroy->entity_id));
        } else {
            event_manager_->emit(EntityDestroyEvent(destroy->entity_id));
        }
    }
}

void NetworkService::handle_player_join(const RType::Protocol::PacketHeader& header, const uint8_t* payload) {
    // For now, assume PLAYER_JOIN has player_id + position (we may need to check protocol struct)
    if (header.payload_size >= sizeof(int) + 2 * sizeof(float)) {
        const int* player_id = reinterpret_cast<const int*>(payload);
        const float* position_data = reinterpret_cast<const float*>(payload + sizeof(int));

        // Only emit event for other players, not ourselves
        if (*player_id != local_player_id_) {
            event_manager_->emit(PlayerJoinEvent(*player_id, position_data[0], position_data[1]));
        }
    }
}

void NetworkService::handle_player_leave(const RType::Protocol::PacketHeader& header, const uint8_t* payload) {
    if (header.payload_size >= sizeof(int)) {
        const int* player_id = reinterpret_cast<const int*>(payload);

        // Only emit event for other players, not ourselves
        if (*player_id != local_player_id_) {
            event_manager_->emit(PlayerLeaveEvent(*player_id));
        }
    }
}

uint32_t NetworkService::get_current_timestamp() {
    return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
}