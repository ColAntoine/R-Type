/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Enemy Cleanup System
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Components.hpp"
#include "Entity/Components/Enemy/Enemy.hpp"
#include <memory>

// Forward declaration
class NetworkSyncSystem;

class EnemyCleanupSystem : public ISystem {
private:
    NetworkSyncSystem* network_sync_system_{nullptr};

    float world_bounds_left_{-100.0f};
    float world_bounds_right_{1500.0f};
    float world_bounds_top_{-100.0f};
    float world_bounds_bottom_{900.0f};

public:
    EnemyCleanupSystem() = default;
    explicit EnemyCleanupSystem(NetworkSyncSystem* net_sync);

    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "EnemyCleanupSystem"; }

    void set_network_sync_system(NetworkSyncSystem* net_sync) {
        network_sync_system_ = net_sync;
    }
    void set_world_bounds(float left, float right, float top, float bottom) {
        world_bounds_left_ = left;
        world_bounds_right_ = right;
        world_bounds_top_ = top;
        world_bounds_bottom_ = bottom;
    }
};


#if defined(_WIN32)
  #define DLL_EXPORT extern "C" __declspec(dllexport)
#else
  #define DLL_EXPORT extern "C"
#endif

DLL_EXPORT ISystem* create_system();
DLL_EXPORT void     destroy_system(ISystem* ptr);