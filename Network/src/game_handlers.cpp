#include "game_handlers.hpp"
#include "session.hpp"
#include "udp_server.hpp"
#include "protocol.hpp"
#include <iostream>
#include <chrono>

namespace RType::Network {

// ============================================================================
// ConnectionHandler Implementation
// ============================================================================

bool ConnectionHandler::handle_typed_message(
    std::shared_ptr<Session> session,
    const Protocol::ClientConnect& message,
    const char* payload,
    size_t payload_size) {

    std::cout << "Client connection request from session: " << session->get_session_id() << std::endl;
    std::cout << "Player name: " << std::string(message.player_name, 32) << std::endl;
    std::cout << "Client version: " << message.client_version << std::endl;

    // Assign player ID and mark as authenticated
    static std::atomic<int> next_player_id{1};
    int player_id = next_player_id++;
    session->set_player_id(player_id);
    session->set_authenticated(true);
    session->set_position(100.0f, 100.0f); // Default spawn position

    // Send acceptance response
    Protocol::ServerAccept response;
    response.player_id = player_id;
    response.session_id = std::hash<std::string>{}(session->get_session_id()); // Simple hash
    response.spawn_x = 100.0f;
    response.spawn_y = 100.0f;

    auto packet = Protocol::create_packet(
        static_cast<uint8_t>(Protocol::SystemMessage::SERVER_ACCEPT),
        response
    );

    session->send(reinterpret_cast<const char*>(packet.data()), packet.size());

    std::cout << "Assigned player ID " << player_id << " to session " << session->get_session_id() << std::endl;

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