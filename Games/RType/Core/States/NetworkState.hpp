/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Shared Network State for Client Connection
*/

#pragma once

#include "Core/Client/Network/UDPClient.hpp"
#include <memory>

struct NetworkState {
    std::shared_ptr<UdpClient> udp_client;
    uint32_t player_id{0};
    bool connected{false};
};
