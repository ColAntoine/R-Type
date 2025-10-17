#pragma once

#include <string>
#include <asio.hpp>
#include <vector>
#include <optional>
#include <thread>

#include "Core/Server/Protocol/Protocol.hpp"

class UdpClient {
    public:
        UdpClient();
        ~UdpClient();

        // Attempt to connect to server (send CLIENT_CONNECT and wait for SERVER_ACCEPT)
        // Returns ServerAccept on success, std::nullopt on failure/timeout.
        std::optional<RType::Protocol::ServerAccept> connect(const std::string &server_ip, uint16_t server_port, const std::string &player_name, uint32_t client_version = 1, int timeout_ms = 2000);

        void disconnect();

    private:
        asio::io_context io_context_;
        asio::ip::udp::socket socket_;
        asio::ip::udp::endpoint server_endpoint_;
};