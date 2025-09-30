#include "udp_server.hpp"
#include "session.hpp"
#include "protocol.hpp"
#include <iostream>

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
            socket_.set_option(asio::ip::udp::socket::reuse_address(true));
            socket_.bind(endpoint);

            running_ = true;

            // Start receiving messages
            start_receive();

            // Start cleanup timer
            setup_cleanup_timer();

            std::cout << "UdpServer started on port " << port_ << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to start UdpServer: " << e.what() << std::endl;
            running_ = false;
            return false;
        }
    }

    void UdpServer::stop() {
        if (!running_) {
            return;
        }

        std::cout << "Stopping UdpServer..." << std::endl;

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

        std::cout << "UdpServer stopped" << std::endl;
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

    std::shared_ptr<Session> UdpServer::get_session(const std::string& connection_id) {
        auto it = sessions_.find(connection_id);
        return (it != sessions_.end()) ? it->second : nullptr;
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
        // Get or create session for this endpoint
        auto session = get_or_create_session(remote_endpoint_);

        if (session && message_handler_) {
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
                    // Call the dispatcher with a simple format: message_type + payload
                    std::vector<char> dispatcher_data;
                    dispatcher_data.reserve(1 + header->payload_size);
                    dispatcher_data.push_back(header->message_type);
                    dispatcher_data.insert(dispatcher_data.end(), payload, payload + header->payload_size);

                    message_handler_(session, dispatcher_data.data(), dispatcher_data.size());
                } else {
                    std::cerr << "Payload size mismatch: expected " << header->payload_size
                              << ", got " << actual_payload_size << std::endl;
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
}    std::shared_ptr<Session> UdpServer::get_or_create_session(const asio::ip::udp::endpoint& endpoint) {
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