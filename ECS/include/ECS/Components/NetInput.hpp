/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** NetInput component (server side)
*/

#pragma once

#include "ECS/Components/IComponent.hpp"
#include <cstdint>
#include <vector>
#include <string>

struct NetInput : public IComponent {
    uint8_t message_type{0};
    std::vector<char> payload; // raw payload bytes (not including message_type)
    std::string session_id;    // origin session id (address:port)

    NetInput() = default;
    NetInput(uint8_t mt, std::vector<char> p, std::string s)
        : message_type(mt), payload(std::move(p)), session_id(std::move(s)) {}
};
