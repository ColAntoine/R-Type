/*
** EPITECH PROJECT, 2025
** asd
** File description:
** asd
*/

#pragma once

#include "ECS/Registry.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/Components/Collider.hpp"
#include "ECS/Components/Sprite.hpp"
#include "ECS/Components/Animation.hpp"

extern "C" {
    void register_components(registry &r);
}
