#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include <asio.hpp>
#include "connection.hpp"

namespace RType::Network {

    class Session;

    /**
     * @brief UDP Server implementation using Asio
     *
     * This class manages UDP socket operations, client connections,
     * and message routing using modern Asio patterns.
     */
    class UdpServer {
    public:
        using endpoint_type = asio::ip::udp::endpoint;
        using message_handler_type = std::function<void(std::shared_ptr<Session>, const char*, size_t)>;

        /**
         * @brief Construct UDP server
         * @param io_context Asio io_context for async operations
         * @param port Port to bind server to
         */
        UdpServer(asio::io_context& io_context, uint16_t port);

        /**
         * @brief Start the server and begin accepting messages
         * @return true if server started successfully
         */
        bool start();

        /**
         * @brief Stop the server
         */
        void stop();

        /**
         * @brief Check if server is running
         */
        bool is_running() const { return running_; }

        /**
         * @brief Set message handler for incoming messages
         * @param handler Function to handle incoming messages
         */
        void set_message_handler(message_handler_type handler) {
            message_handler_ = std::move(handler);
        }

        /**
         * @brief Send data to specific client
         * @param connection_id Client connection ID
         * @param data Data to send
         * @param size Size of data
         */
        void send_to_client(const std::string& connection_id, const char* data, size_t size);

        /**
         * @brief Broadcast data to all connected clients
         * @param data Data to broadcast
         * @param size Size of data
         */
        void broadcast(const char* data, size_t size);

        /**
         * @brief Broadcast data to all connected clients except one
         * @param exclude_session_id Session ID to exclude from broadcast
         * @param data Data to broadcast
         * @param size Size of data
         */
        void broadcast_except(const std::string& exclude_session_id, const char* data, size_t size);

        /**
         * @brief Get number of connected clients
         */
        size_t get_client_count() const;

        /**
         * @brief Get session by connection ID
         */
        std::shared_ptr<Session> get_session(const std::string& connection_id);

        /**
         * @brief Remove disconnected clients (cleanup)
         */
        void cleanup_disconnected_clients();

    private:
        void start_receive();
        void handle_receive(const std::error_code& ec, size_t bytes_received);
        std::shared_ptr<Session> get_or_create_session(const endpoint_type& endpoint);
        void setup_cleanup_timer();

        asio::io_context& io_context_;
        asio::ip::udp::socket socket_;
        asio::ip::udp::endpoint remote_endpoint_;
        asio::steady_timer cleanup_timer_;

        std::array<char, 1024> receive_buffer_;
        std::unordered_map<std::string, std::shared_ptr<Session>> sessions_;
        std::unordered_map<std::string, std::shared_ptr<Connection>> connections_;

        message_handler_type message_handler_;
        bool running_;
        uint16_t port_;
    };

} // namespace RType::Network