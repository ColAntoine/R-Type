/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Lifetime Component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct lifetime : public IComponent {
    float max_lifetime{10.0f};          // Maximum time to live (seconds)
    float current_time{0.0f};           // Current age
    bool destroy_on_collision{false};   // Whether to destroy on collision
    bool destroy_offscreen{true};       // Whether to destroy when offscreen

    lifetime();
    lifetime(float max_time, bool collision_destroy = false, bool offscreen_destroy = true);
};
