#include "UDPServer.hpp"
#include "Session.hpp"
#include "Protocol/Protocol.hpp"
#include <iostream>
#include "ECS/Utils/Console.hpp"
#include <cstring>
#include <chrono>
#include <ctime>
#include <vector>

namespace RType::Network {

    UdpServer::UdpServer(asio::io_context& io_context, uint16_t port)
        : io_context_(io_context)
        , socket_(io_context)
        , cleanup_timer_(io_context)
        , running_(false)
        , port_(port) {
    }

    bool UdpServer::start() {
        if (running_) {
            std::cerr << "UdpServer is already running" << std::endl;
            return false;
        }

        try {
            // Open and bind the socket
            asio::ip::udp::endpoint endpoint(asio::ip::udp::v4(), port_);
            socket_.open(endpoint.protocol());
            socket_.set_option(asio::ip::udp::socket::reuse_address(false)); // Change option to true to reuse socket
            socket_.bind(endpoint);

            running_ = true;

            // Start receiving messages
            start_receive();

            // Start cleanup timer
            setup_cleanup_timer();
            std::cout << Console::green("[UdpServer] ") << "Started on port " << port_ << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << Console::red("[UdpServer] ") << "Failed to start UdpServer: " << e.what() << std::endl;
            running_ = false;
            return false;
        }
    }

    void UdpServer::stop() {
        if (!running_) {
            return;
        }

    std::cout << Console::yellow("[UdpServer] ") << "Stopping..." << std::endl;

        running_ = false;

        // Cancel all async operations
        cleanup_timer_.cancel();

        if (socket_.is_open()) {
            std::error_code ec;
            socket_.close(ec);
        }

        // Clear all sessions and connections
        sessions_.clear();
        connections_.clear();

        std::cout << Console::green("[UdpServer] ") << "Stopped" << std::endl;
    }

    void UdpServer::send_to_client(const std::string& connection_id, const char* data, size_t size) {
        auto it = connections_.find(connection_id);
        if (it != connections_.end() && it->second->is_active()) {
            it->second->send(data, size);
        }
    }

    void UdpServer::broadcast(const char* data, size_t size) {
        for (const auto& [id, connection] : connections_) {
            if (connection->is_active()) {
                connection->send(data, size);
            }
        }
    }

    void UdpServer::broadcast_except(const std::string& exclude_session_id, const char* data, size_t size) {
        for (const auto& [id, connection] : connections_) {
            if (connection->is_active() && id != exclude_session_id) {
                connection->send(data, size);
            }
        }
    }

    size_t UdpServer::get_client_count() const {
        size_t count = 0;
        for (const auto& [id, connection] : connections_) {
            if (connection->is_active()) {
                ++count;
            }
        }
        return count;
    }

    size_t UdpServer::get_ready_client_count() const {
        size_t count = 0;
        for (const auto& [id, session] : sessions_) {
            if (session->is_connected() && session->is_authenticated() && session->is_ready()) {
                ++count;
            }
        }
        return count;
    }

    bool UdpServer::are_all_clients_ready() const {
        if (sessions_.empty()) {
            return false; // No clients connected
        }

        for (const auto& [id, session] : sessions_) {
            if (session->is_connected() && session->is_authenticated()) {
                if (!session->is_ready()) {
                    return false; // Found a connected client that is not ready
                }
            }
        }
        return true; // All connected clients are ready
    }

    bool UdpServer::should_run_game_logic() const {
        size_t connected_count = 0;
        size_t ready_count = 0;

        for (const auto& [id, session] : sessions_) {
            if (session->is_connected() && session->is_authenticated()) {
                connected_count++;
                if (session->is_ready()) {
                    ready_count++;
                }
            }
        }

        // Run game logic only if we have clients connected and all are ready
        return connected_count > 0 && connected_count == ready_count;
    }

    RType::Protocol::ClientListUpdate UdpServer::generate_player_list() {
        Protocol::ClientListUpdate client_list;
        client_list.player_count = 0;

        // Collect all connected and authenticated players
        for (const auto& [id, session] : sessions_) {
            if (session->is_connected() && session->is_authenticated() &&
                client_list.player_count < 8) { // Max 8 players

                auto& player_info = client_list.players[client_list.player_count];
                player_info.player_id = session->get_player_id();
                player_info.ready_state = session->is_ready() ? 1 : 0;

                // Use the custom player name stored in the session
                const std::string& custom_name = session->get_player_name();
                if (!custom_name.empty()) {
                    strncpy(player_info.name, custom_name.c_str(), 31);
                } else {
                    // Fallback to default format if no custom name
                    strncpy(player_info.name, ("Player " + std::to_string(session->get_player_id())).c_str(), 31);
                }
                player_info.name[31] = '\0'; // Ensure null termination

                client_list.player_count++;
            }
        }

        return client_list;
    }

    void UdpServer::broadcast_player_list() {
        auto client_list = generate_player_list();

        // Create packet using utility function
        auto packet = Protocol::create_packet(
            static_cast<uint8_t>(Protocol::SystemMessage::CLIENT_LIST),
            client_list
        );

        // Broadcast to all clients
        broadcast(reinterpret_cast<const char*>(packet.data()), packet.size());
    }

    void UdpServer::send_player_list_to_client(const std::string& session_id) {
        auto session = get_session(session_id);
        if (!session || !session->is_connected() || !session->is_authenticated()) {
            return;
        }

        auto client_list = generate_player_list();

        // Create packet using utility function
        auto packet = Protocol::create_packet(
            static_cast<uint8_t>(Protocol::SystemMessage::CLIENT_LIST),
            client_list
        );

        // Send to specific client
        session->send(reinterpret_cast<const char*>(packet.data()), packet.size());
    }

    void UdpServer::check_all_players_ready() {
        uint32_t connected_players = 0;
        uint32_t ready_players = 0;

        // Count connected and ready players
        for (const auto& [session_id, session] : sessions_) {
            if (session->is_connected() && session->is_authenticated()) {
                connected_players++;
                if (session->is_ready()) {
                    ready_players++;
                }
            }
        }

        // Need at least 1 player and all connected players must be ready
        if (connected_players > 0 && ready_players == connected_players) {
            // Create START_GAME message
            Protocol::StartGame start_game;
            start_game.timestamp = static_cast<uint32_t>(std::time(nullptr));

            auto packet = Protocol::create_packet(
                static_cast<uint8_t>(Protocol::SystemMessage::START_GAME),
                start_game
            );

            // Broadcast to all connected clients
            broadcast(reinterpret_cast<const char*>(packet.data()), packet.size());
        }
    }

    std::shared_ptr<Session> UdpServer::get_session(const std::string& connection_id) {
        auto it = sessions_.find(connection_id);
        return (it != sessions_.end()) ? it->second : nullptr;
    }

    std::shared_ptr<Session> UdpServer::get_session_by_player_name(const std::string& name) {
        for (const auto& [id, session] : sessions_) {
            if (session && session->is_connected() && session->is_authenticated()) {
                if (session->get_player_name() == name) {
                    return session;
                }
            }
        }
        return nullptr;
    }

    bool UdpServer::disconnect_session_by_player_name(const std::string& name) {
        auto it = std::find_if(sessions_.begin(), sessions_.end(), [&](const auto& kv) {
            const auto& session = kv.second;
            return session && session->get_player_name() == name;
        });

        if (it != sessions_.end()) {
            it->second->disconnect();
            sessions_.erase(it);
            return true;
        }
        return false;
    }

    void UdpServer::cleanup_disconnected_clients() {
        auto session_it = sessions_.begin();
        while (session_it != sessions_.end()) {
            if (!session_it->second->is_connected() ||
                session_it->second->is_timed_out(std::chrono::minutes(5))) {
                std::cout << "Cleaning up disconnected session: " << session_it->first << std::endl;
                session_it = sessions_.erase(session_it);
            } else {
                ++session_it;
            }
        }

        auto conn_it = connections_.begin();
        while (conn_it != connections_.end()) {
            if (!conn_it->second->is_active() ||
                conn_it->second->get_idle_time() > std::chrono::minutes(5)) {
                std::cout << "Cleaning up disconnected connection: " << conn_it->first << std::endl;
                conn_it = connections_.erase(conn_it);
            } else {
                ++conn_it;
            }
        }
    }

    void UdpServer::start_receive() {
        if (!running_) return;

        socket_.async_receive_from(
            asio::buffer(receive_buffer_),
            remote_endpoint_,
            [this](const std::error_code& ec, size_t bytes_received) {
                handle_receive(ec, bytes_received);
            }
        );
    }

    void UdpServer::handle_receive(const std::error_code& ec, size_t bytes_received) {
        if (!running_) return;

        if (!ec && bytes_received > 0) {
            auto session = get_or_create_session(remote_endpoint_);

            if (session) {
                session->update_activity();

                // Parse packet header first
                if (bytes_received >= sizeof(RType::Protocol::PacketHeader)) {
                    const RType::Protocol::PacketHeader* header =
                        reinterpret_cast<const RType::Protocol::PacketHeader*>(receive_buffer_.data());

                    // Calculate payload start and size
                    const char* payload = receive_buffer_.data() + sizeof(RType::Protocol::PacketHeader);
                    size_t actual_payload_size = bytes_received - sizeof(RType::Protocol::PacketHeader);

                    // Verify payload size matches header
                    if (actual_payload_size >= header->payload_size) {
                        // Build dispatcher_data: message_type + payload
                        std::vector<char> dispatcher_data;
                        dispatcher_data.reserve(1 + header->payload_size);
                        dispatcher_data.push_back(header->message_type);
                        dispatcher_data.insert(dispatcher_data.end(), payload, payload + header->payload_size);

                        // If a message_queue_ is set, enqueue the packet for the server ECS
                        if (message_queue_) {
                            ReceivedPacket pkt;
                            pkt.session_id = remote_endpoint_.address().to_string() + ":" + std::to_string(remote_endpoint_.port());
                            pkt.data = std::move(dispatcher_data);
                            message_queue_->push(std::move(pkt));
                        } else if (message_handler_) {
                            // Backwards compatible: call the handler directly
                            message_handler_(session, dispatcher_data.data(), dispatcher_data.size());
                        }
                    } else {
                        // Check if this might be a disconnect scenario (payload size 0)
                        if (actual_payload_size == 0) {
                            std::cout << "Client " << remote_endpoint_.address().to_string()
                                      << ":" << remote_endpoint_.port() << " appears to have disconnected (zero payload)" << std::endl;
                            if (session) {
                                session->disconnect();
                            }
                            // Inform server ECS via message queue that this session disconnected
                            if (message_queue_) {
                                ReceivedPacket pkt;
                                pkt.session_id = remote_endpoint_.address().to_string() + ":" + std::to_string(remote_endpoint_.port());
                                pkt.data.push_back(static_cast<char>(static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_DISCONNECT)));
                                message_queue_->push(std::move(pkt));
                            }
                        } else {
                            std::cerr << "Payload size mismatch: expected " << header->payload_size
                                      << ", got " << actual_payload_size << " from "
                                      << remote_endpoint_.address().to_string() << ":" << remote_endpoint_.port() << std::endl;
                        }
                    }
                } else {
                    std::cerr << "Packet too small for header: " << bytes_received << " bytes" << std::endl;
                }
            }
        } else if (ec != asio::error::operation_aborted) {
            std::cerr << "Receive error: " << ec.message() << std::endl;
        }

        // Continue receiving
        start_receive();
    }

    std::shared_ptr<Session> UdpServer::get_or_create_session(const asio::ip::udp::endpoint& endpoint) {
        // Create connection ID from endpoint
        std::string connection_id = endpoint.address().to_string() + ":" + std::to_string(endpoint.port());

        // Check if session already exists
        auto session_it = sessions_.find(connection_id);
        if (session_it != sessions_.end()) {
            return session_it->second;
        }

        // Create new connection and session
        auto socket_ptr = std::shared_ptr<asio::ip::udp::socket>(&socket_, [](asio::ip::udp::socket*){});
        auto connection = std::make_shared<Connection>(endpoint, socket_ptr);
        auto session = std::make_shared<Session>(connection);

        // Store connection and session
        connections_[connection_id] = connection;
        sessions_[connection_id] = session;

        std::cout << "New client connected: " << connection_id << std::endl;

        return session;
    }

    void UdpServer::setup_cleanup_timer() {
        if (!running_) return;

        cleanup_timer_.expires_after(std::chrono::seconds(30));
        cleanup_timer_.async_wait([this](const std::error_code& ec) {
            if (!ec && running_) {
                cleanup_disconnected_clients();
                setup_cleanup_timer(); // Reschedule
            }
        });
    }

} // namespace RType::Network
