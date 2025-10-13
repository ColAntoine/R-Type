/*
** EPITECH PROJECT, 2025
** asd
** File description:
** da
*/

#include "Entity/Components/Enemy/Enemy.hpp"

enemy::enemy(EnemyAIType type) : enemy_type(type) {}
enemy::enemy(uint8_t type) : enemy_type(static_cast<EnemyAIType>(type)) {}