/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Shared Network State for Client Connection
*/

#pragma once

#include "Core/Client/Network/UDPClient.hpp"
#include "Core/Server/Protocol/MessageQueue.hpp"
#include <memory>
#include <thread>
#include <atomic>

struct NetworkState {
    std::shared_ptr<UdpClient> udp_client;
    std::unique_ptr<RType::Network::MessageQueue> message_queue;
    uint32_t player_id{0};
    bool connected{false};
    std::thread receive_thread;
    std::atomic<bool> receiving{false};
};
