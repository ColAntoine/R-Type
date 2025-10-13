/*
** EPITECH PROJECT, 2025
** asd
** File description:
** da
*/

#include "Entity/Components/Enemy/Enemy.hpp"

Enemy::Enemy(Enemy::EnemyAIType type) : enemy_type(type) {}
Enemy::Enemy(uint8_t type) : enemy_type(static_cast<Enemy::EnemyAIType>(type)) {}

// Compatibility constructors for the old alias name (if some translation units use `enemy::enemy`)
// These are inline wrappers that forward to the new class; they are optional but harmless.
// If code expects free functions or different symbols, the alias `using enemy = Enemy;` in the header
// should be sufficient for most compilers.