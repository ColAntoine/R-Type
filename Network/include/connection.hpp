#pragma once

#include <memory>
#include <string>
#include <chrono>
#include <asio.hpp>

namespace RType::Network {

    /**
     * @brief Represents a client connection with state management
     *
     * This class encapsulates client connection information and provides
     * methods for sending data and managing connection state.
     */
    class Connection {
        public:
            using endpoint_type = asio::ip::udp::endpoint;
            using clock_type = std::chrono::steady_clock;

            /**
             * @brief Construct a new Connection
             * @param endpoint Client's UDP endpoint
             * @param socket Shared UDP socket for communication
             */
            Connection(const endpoint_type& endpoint, std::shared_ptr<asio::ip::udp::socket> socket);

            /**
             * @brief Get the client's endpoint
             */
            const endpoint_type& get_endpoint() const { return endpoint_; }

            /**
             * @brief Get unique connection ID
             */
            const std::string& get_id() const { return connection_id_; }

            /**
             * @brief Send data to this connection
             * @param data Data buffer to send
             * @param size Size of data to send
             */
            void send(const char* data, size_t size);

            /**
             * @brief Send data to this connection (async)
             * @param data Data buffer to send
             * @param size Size of data to send
             * @param handler Completion handler
             */
            template<typename CompletionHandler>
            void async_send(const char* data, size_t size, CompletionHandler&& handler);

            /**
             * @brief Check if connection is active
             */
            bool is_active() const;

            /**
             * @brief Update last activity timestamp
             */
            void update_activity();

            /**
             * @brief Get time since last activity
             */
            std::chrono::milliseconds get_idle_time() const;

            /**
             * @brief Mark connection as disconnected
             */
            void disconnect();

        private:
            std::string generate_connection_id(const endpoint_type& endpoint);

            endpoint_type endpoint_;
            std::shared_ptr<asio::ip::udp::socket> socket_;
            std::string connection_id_;
            clock_type::time_point last_activity_;
            bool active_;
    };

    // Template implementation
    template<typename CompletionHandler>
    void Connection::async_send(const char* data, size_t size, CompletionHandler&& handler) {
        if (!active_) {
            // Post error to handler
            asio::post(socket_->get_executor(), [handler = std::forward<CompletionHandler>(handler)]() {
                handler(asio::error::not_connected, 0);
            });
            return;
        }

        auto buffer = std::make_shared<std::vector<char>>(data, data + size);
        socket_->async_send_to(
            asio::buffer(*buffer),
            endpoint_,
            [buffer, handler = std::forward<CompletionHandler>(handler)]
            (std::error_code ec, size_t bytes_sent) {
                handler(ec, bytes_sent);
            }
        );
    }

} // namespace RType::Network