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
    uint32_t network_id{0};           // ID réseau unique
    float broadcast_timer{0.0f};       // Timer pour broadcast périodique
    float broadcast_interval{0.1f};    // 10 updates/sec
    bool dirty{false};                 // Nécessite un broadcast

    network_sync() = default;
    explicit network_sync(uint32_t id) : network_id(id) {}

    void mark_dirty() { dirty = true; }
    bool needs_broadcast() const { return dirty || broadcast_timer >= broadcast_interval; }
};