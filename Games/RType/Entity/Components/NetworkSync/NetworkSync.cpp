/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Network Synchronization Component Implementation
*/

#include "Entity/Components/NetworkSync/NetworkSync.hpp"

network_sync::network_sync(uint32_t id) : network_id(id) {}
void network_sync::mark_dirty() { dirty = true; }
bool network_sync::needs_broadcast() const { return dirty || broadcast_timer >= broadcast_interval; }