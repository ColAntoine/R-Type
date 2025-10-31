#pragma once

#include <cstddef>
#include <unordered_map>
#include "ECS/Registry.hpp"
#include "ECS/DLLoader.hpp"
#include "ECS/Entity.hpp"

class EnemyHandler {
public:
    EnemyHandler(registry& registry, DLLoader& loader);

    // Handle enemy spawn (ENTITY_CREATE messages for enemies)
    void on_enemy_spawn(const char* payload, size_t size);

private:
    registry& registry_;
    DLLoader& loader_;

    // Map server entity ID to local client entity
    std::unordered_map<uint32_t, entity> enemy_map_;
};
