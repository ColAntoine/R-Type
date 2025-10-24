#pragma once

#include <string>
#include <asio.hpp>
#include <vector>
#include <optional>
#include <thread>
#include <atomic>

#include "Core/Server/Protocol/Protocol.hpp"

class UdpClient {
    public:
        UdpClient();
        ~UdpClient();

        // Attempt to connect to server (send CLIENT_CONNECT and wait for SERVER_ACCEPT)
        // Returns ServerAccept on success, std::nullopt on failure/timeout.
        std::optional<RType::Protocol::ServerAccept> connect(const std::string &server_ip, uint16_t server_port, const std::string &player_name, uint32_t client_version = 1, int timeout_ms = 2000);

        void disconnect();

        // After successful connect, the client stores the assigned numeric session token
        void set_session_token(uint32_t token) { session_token_ = token; }
        uint32_t get_session_token() const { return session_token_; }

        // Send a voluntary disconnect message to the server (best-effort)
        void send_disconnect(uint32_t player_id);

    // Start/stop background receive loop. Handler invoked on each received message: (message_type, payload, size)
    void start_receive_loop(std::function<void(uint8_t,const char*,size_t)> handler);
    void stop_receive_loop(bool close_socket = true);

        // Helper to send an input packet (auto-includes session token and input sequence)
    private:
        asio::io_context io_context_;
        asio::ip::udp::socket socket_;
        asio::ip::udp::endpoint server_endpoint_;
        uint32_t session_token_{0};
        std::atomic<bool> recv_running_{false};
        std::thread recv_thread_;
        std::function<void(uint8_t,const char*,size_t)> recv_handler_;
};