/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Network Synchronization Component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"
#include <cstdint>

struct network_sync : public IComponent {
    uint32_t network_id{0};           // Unique network ID
    float broadcast_timer{0.0f};       // Timer for periodic broadcast
    float broadcast_interval{0.1f};    // 10 updates/sec
    bool dirty{false};                 // Requires broadcast

    network_sync() = default;
    explicit network_sync(uint32_t id);

    void mark_dirty();
    bool needs_broadcast() const;
};