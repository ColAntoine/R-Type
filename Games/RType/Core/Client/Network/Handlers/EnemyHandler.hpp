#pragma once

#include <cstddef>
#include <unordered_map>
#include "ECS/Registry.hpp"
#include "ECS/ILoader.hpp"
#include "ECS/Entity.hpp"

class EnemyHandler {
public:
    EnemyHandler(registry& registry, ILoader& loader);
    
    void on_enemy_spawn(const char* payload, size_t size);

private:
    registry& registry_;
    ILoader& loader_;
    
    // Map server entity ID to local client entity
    std::unordered_map<uint32_t, entity> enemy_map_;
};
