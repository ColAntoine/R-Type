/*
** EPITECH PROJECT, 2025
** asd
** File description:
** da
*/

#include "Entity/Components/Enemy/Enemy.hpp"

Enemy::Enemy(Enemy::EnemyAIType type) : enemy_type(type) {}
Enemy::Enemy(uint8_t type) : enemy_type(static_cast<Enemy::EnemyAIType>(type)) {}
