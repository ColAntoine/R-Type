#pragma once

#include <cstddef>

class EnemyHandler {
    public:
        EnemyHandler();
    void on_enemy_spawn(const char* payload, size_t size);
};
