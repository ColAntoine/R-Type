#include "game_handlers.hpp"
#include "session.hpp"
#include "udp_server.hpp"
#include "protocol.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstring>

namespace RType::Network {

// ============================================================================
// ConnectionHandler Implementation
// ============================================================================

bool ConnectionHandler::handle_typed_message(
    std::shared_ptr<Session> session,
    const Protocol::ClientConnect& message,
    const char* payload,
    size_t payload_size) {

    // Safely extract player name with null termination
    char safe_player_name[33]; // 32 + 1 for null terminator
    memcpy(safe_player_name, message.player_name, 32);
    safe_player_name[32] = '\0'; // Ensure null termination
    std::string player_name_str(safe_player_name);

    // Assign player ID and mark as authenticated
    static std::atomic<int> next_player_id{1};
    int player_id = next_player_id++;
    session->set_player_id(player_id);
    session->set_player_name(player_name_str);  // Store the custom player name
    session->set_authenticated(true);
    
    // Calculate spawn position based on player ID to avoid collisions
    float spawn_x = 100.0f; // Same X position (left side of screen)
    float spawn_y = 200.0f + (player_id * 80.0f); // Different Y positions, spaced 80 pixels apart
    session->set_position(spawn_x, spawn_y);

    // Send acceptance response
    Protocol::ServerAccept response;
    response.player_id = player_id;
    response.session_id = std::hash<std::string>{}(session->get_session_id()); // Simple hash
    response.spawn_x = spawn_x;
    response.spawn_y = spawn_y;

    auto packet = Protocol::create_packet(
        static_cast<uint8_t>(Protocol::SystemMessage::SERVER_ACCEPT),
        response
    );

    session->send(reinterpret_cast<const char*>(packet.data()), packet.size());

    // Send current player list to the newly connected client first
    // Then broadcast updated player list to all clients
    if (server_) {
        // Give the client a moment to process the SERVER_ACCEPT and transition to WaitingLobby state
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        server_->send_player_list_to_client(session->get_session_id());
        // Also broadcast to all clients so they see the new player
        server_->broadcast_player_list();
    }

    return true;
}

// ============================================================================
// ClientReadyHandler Implementation
// ============================================================================

bool ClientReadyHandler::handle_typed_message(
    std::shared_ptr<Session> session,
    const RType::Protocol::ClientReady& message,
    const char* payload,
    size_t payload_size) {

    if (!session->is_authenticated()) {
        return false;
    }

    // Verify the player ID matches the session
    if (message.player_id != static_cast<uint32_t>(session->get_player_id())) {
        return false;
    }

    // Update the ready state
    bool ready_state = (message.ready_state == 1);
    session->set_ready(ready_state);

    // Broadcast updated player list to all clients
    if (server_) {
        server_->broadcast_player_list();
        server_->check_all_players_ready();
    }

    return true;
}

// ============================================================================
// ClientDisconnectHandler Implementation
// ============================================================================

bool ClientDisconnectHandler::handle_typed_message(
    std::shared_ptr<Session> session,
    const RType::Protocol::ClientDisconnect& message,
    const char* payload,
    size_t payload_size) {

    if (!session || !session->is_authenticated()) {
        return false;
    }

    // Verify the player ID matches the session
    if (message.player_id != static_cast<uint32_t>(session->get_player_id())) {
        return false;
    }

    std::cout << "Player " << message.player_id << " disconnecting (reason: " << (int)message.reason << ")" << std::endl;

    // Mark session as disconnected
    session->disconnect();

    // Broadcast updated player list to all remaining clients
    if (server_) {
        server_->broadcast_player_list();
    }

    return true;
}

// ============================================================================
// PositionUpdateHandler Implementation
// ============================================================================

bool PositionUpdateHandler::handle_typed_message(
    std::shared_ptr<Session> session,
    const RType::Protocol::PositionUpdate& message,
    const char* payload,
    size_t payload_size) {

    if (!session->is_authenticated()) {
        std::cerr << "Position update from unauthenticated session: " << session->get_session_id() << std::endl;
        return false;
    }

    // Update session position
    session->set_position(message.x, message.y);

    // Create a response message with the updated position for broadcasting
    RType::Protocol::PositionUpdate broadcast_message = message;
    broadcast_message.entity_id = session->get_player_id(); // Use the actual player ID

    // Create a packet for broadcasting
    auto packet = RType::Protocol::create_packet(
        static_cast<uint8_t>(RType::Protocol::GameMessage::POSITION_UPDATE),
        broadcast_message
    );

    // Broadcast to all other clients (excluding the sender)
    if (server_) {
        server_->broadcast_except(session->get_session_id(),
                                 reinterpret_cast<const char*>(packet.data()),
                                 packet.size());
    }

    return true;
}

// ============================================================================
// PingHandler Implementation
// ============================================================================

bool PingHandler::handle_typed_message(
    std::shared_ptr<Session> session,
    const RType::Protocol::Ping& message,
    const char* payload,
    size_t payload_size) {

    // Respond with pong
    RType::Protocol::Pong response;
    response.timestamp = message.timestamp;
    response.sequence = message.sequence;
    response.server_time = static_cast<uint32_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count()
    );

    auto packet = RType::Protocol::create_packet(
        static_cast<uint8_t>(RType::Protocol::SystemMessage::PONG),
        response
    );

    session->send(reinterpret_cast<const char*>(packet.data()), packet.size());

    std::cout << "Ping/Pong with session " << session->get_session_id()
              << " (seq: " << message.sequence << ")" << std::endl;

    return true;
}

// ============================================================================
// PlayerShootHandler Implementation
// ============================================================================

bool PlayerShootHandler::handle_typed_message(
    std::shared_ptr<Session> session,
    const RType::Protocol::PlayerShoot& message,
    const char* payload,
    size_t payload_size) {

    if (!session->is_authenticated()) {
        std::cerr << "Shoot message from unauthenticated session: " << session->get_session_id() << std::endl;
        return false;
    }

    std::cout << "Player " << session->get_player_id() << " shoots!" << std::endl;
    std::cout << "  Start: (" << message.start_x << ", " << message.start_y << ")" << std::endl;
    std::cout << "  Direction: (" << message.dir_x << ", " << message.dir_y << ")" << std::endl;
    std::cout << "  Weapon: " << static_cast<int>(message.weapon_type) << std::endl;

    // Here you would typically:
    // 1. Create a bullet entity
    // 2. Broadcast to all players
    // 3. Start bullet simulation

    return true;
}

} // namespace RType::Network