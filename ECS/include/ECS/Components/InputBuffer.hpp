/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** InputBuffer component - per-session input aggregation
*/

#pragma once

#include "ECS/Components/IComponent.hpp"
#include <cstdint>
#include <vector>
#include <string>

struct Input {
    uint8_t message_type{0};
    std::vector<char> payload; // raw payload bytes (not including message_type)
    uint64_t timestamp{0};     // server-received timestamp (ms)
};

struct InputBuffer : public IComponent {
    std::vector<Input> inputs;
    InputBuffer() = default;
};
