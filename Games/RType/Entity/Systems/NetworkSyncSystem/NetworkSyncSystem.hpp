/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Network Synchronization System
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Components.hpp"
#include "Entity/Components/Enemy/Enemy.hpp"
#include "Entity/Components/NetworkSync/NetworkSync.hpp"
#include "protocol.hpp"
#include "udp_server.hpp"
#include <memory>

class NetworkSyncSystem : public ISystem {
private:
    std::shared_ptr<RType::Network::UdpServer> server_;

public:
    NetworkSyncSystem() = default;
    explicit NetworkSyncSystem(std::shared_ptr<RType::Network::UdpServer> server);

    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "NetworkSyncSystem"; }

    void set_server(std::shared_ptr<RType::Network::UdpServer> server) {
        server_ = server;
    }

    void broadcast_spawn(registry& r, entity e);
    void broadcast_destroy(uint32_t network_id);
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}