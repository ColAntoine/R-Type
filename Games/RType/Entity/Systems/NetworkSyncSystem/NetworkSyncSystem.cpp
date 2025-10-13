/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Network Synchronization System Implementation
*/

#include "NetworkSyncSystem.hpp"
#include <iostream>

NetworkSyncSystem::NetworkSyncSystem(std::shared_ptr<RType::Network::UdpServer> server)
    : server_(server)
{
}

void NetworkSyncSystem::update(registry& r, float dt) {
    if (!server_) return;

    auto* sync_comps = r.get_if<network_sync>();
    auto* positions = r.get_if<position>();

    if (!sync_comps || !positions) return;

    auto* enemies = r.get_if<Enemy>();

    // Broadcast entity updates
    for (auto [sync, pos, index] : zipper(*sync_comps, *positions)) {
        sync.broadcast_timer += dt;
        if (sync.needs_broadcast()) {
            uint8_t enemy_type = 1;
            if (enemies && index < enemies->size()) {
                enemy_type = static_cast<uint8_t>((*enemies)[index].enemy_type);
            }

            RType::Protocol::PositionUpdate update_msg;
            update_msg.entity_id = sync.network_id;
            update_msg.x = pos.x;
            update_msg.y = pos.y;
            update_msg.vx = 0.0f;
            update_msg.vy = 0.0f;
            update_msg.timestamp = static_cast<uint32_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count()
            );

            auto packet = RType::Protocol::create_packet(
                static_cast<uint8_t>(RType::Protocol::GameMessage::ENTITY_UPDATE),
                update_msg
            );
            server_->broadcast(reinterpret_cast<const char*>(packet.data()), 
                              packet.size());
            sync.broadcast_timer = 0.0f;
            sync.dirty = false;
        }
    }
}

void NetworkSyncSystem::broadcast_spawn(registry& r, entity e) {
    if (!server_) return;

    auto* sync_comps = r.get_if<network_sync>();
    auto* positions = r.get_if<position>();
    auto* enemies = r.get_if<Enemy>();

    if (!sync_comps || !positions) return;
    if (e >= sync_comps->size() || e >= positions->size()) return;

    auto& sync = (*sync_comps)[e];
    auto& pos = (*positions)[e];
    uint8_t type = enemies && e < enemies->size() ?
                  static_cast<uint8_t>((*enemies)[e].enemy_type) : 1;
    RType::Protocol::EntityCreate spawn_msg;
    spawn_msg.entity_id = sync.network_id;
    spawn_msg.entity_type = type;
    spawn_msg.x = pos.x;
    spawn_msg.y = pos.y;
    spawn_msg.health = 0.0f;

    auto packet = RType::Protocol::create_packet(
        static_cast<uint8_t>(RType::Protocol::GameMessage::ENTITY_CREATE),
        spawn_msg
    );
    server_->broadcast(reinterpret_cast<const char*>(packet.data()), 
                      packet.size());
    std::cout << "Broadcast spawn: enemy " << sync.network_id 
              << " at (" << pos.x << ", " << pos.y << ")" << std::endl;
}

void NetworkSyncSystem::broadcast_destroy(uint32_t network_id) {
    if (!server_) return;
    RType::Protocol::EntityDestroy destroy_msg;
    destroy_msg.entity_id = network_id;
    auto packet = RType::Protocol::create_packet(
        static_cast<uint8_t>(RType::Protocol::GameMessage::ENTITY_DESTROY),
        destroy_msg
    );
    server_->broadcast(reinterpret_cast<const char*>(packet.data()), 
                      packet.size());
    std::cout << "Broadcast destroy: enemy " << network_id << std::endl;
}

extern "C" {
    std::unique_ptr<ISystem> create_system() {
        return std::make_unique<NetworkSyncSystem>();
    }
}