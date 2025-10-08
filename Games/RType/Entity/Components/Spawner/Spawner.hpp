/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Spawner Component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct spawner : public IComponent {
    float spawn_interval{2.0f};   // Time between spawns (seconds)
    float last_spawn_time{0.0f};  // Time since last spawn
    int max_entities{10};         // Maximum concurrent entities
    int current_count{0};         // Current spawned count

    spawner();
    spawner(float interval, int max_ents = 10);
};
