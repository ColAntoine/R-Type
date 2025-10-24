#include "EnemyHandler.hpp"
#include <iostream>

EnemyHandler::EnemyHandler() {}

void EnemyHandler::on_enemy_spawn(const char* payload, size_t size) {
    (void)payload; (void)size;
    std::cout << "[EnemyMsg] Received enemy spawn (stub handler)" << std::endl;
}
