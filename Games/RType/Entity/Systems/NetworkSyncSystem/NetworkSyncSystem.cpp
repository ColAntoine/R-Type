    // --- Lag compensation stub ---
    static std::unordered_map<uint32_t, std::deque<position>> client_position_history;
    const size_t history_length = 60; // 1 second at 60Hz
    for (auto [sync, pos, index] : zipper(*sync_comps, *positions)) {
        client_position_history[sync.network_id].push_back(pos);
        if (client_position_history[sync.network_id].size() > history_length)
            client_position_history[sync.network_id].pop_front();
        // TODO: Use history for lag compensation in hit detection
    }
    // --- ACK handling and resend logic ---
    static std::unordered_map<uint32_t, uint32_t> last_sent_sequence;
    static std::unordered_map<uint32_t, RType::Protocol::PositionUpdate> unconfirmed_updates;
    for (auto [sync, pos, index] : zipper(*sync_comps, *positions)) {
        uint32_t seq = last_sent_sequence[sync.network_id] + 1;
        last_sent_sequence[sync.network_id] = seq;
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
        // Store for resend if not acknowledged
        unconfirmed_updates[seq] = update_msg;
    }

    // Resend unconfirmed updates (stub)
    // TODO: Check for ACKs and resend if needed
/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Network Synchronization System Implementation
*/

#include "NetworkSyncSystem.hpp"
#include <iostream>

// TODO: IS NOT ALREADY SYNC WITH THE SERVER BUT WE SHOULD USE IT TO COMMUNICATE

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

    // Delta-compression: only broadcast if state changed
    static std::unordered_map<uint32_t, position> last_sent_positions;
    for (auto [sync, pos, index] : zipper(*sync_comps, *positions)) {
        sync.broadcast_timer += dt;
        bool changed = false;
        auto it = last_sent_positions.find(sync.network_id);
        if (it == last_sent_positions.end() ||
            it->second.x != pos.x || it->second.y != pos.y) {
            changed = true;
        }
        if (sync.needs_broadcast() || changed) {
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
            last_sent_positions[sync.network_id] = pos;
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