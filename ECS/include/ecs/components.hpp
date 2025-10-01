/*
** EPITECH PROJECT, 2025
** asd
** File description:
** asd
*/

#pragma once

#include "ecs/registry.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/velocity.hpp"
#include "ecs/components/drawable.hpp"
#include "ecs/components/controllable.hpp"
#include "ecs/components/collider.hpp"
#include "ecs/components/remote_player.hpp"
#include "ecs/components/enemy.hpp"
#include "ecs/components/lifetime.hpp"
#include "ecs/components/spawner.hpp"

extern "C" {
    void register_components(registry &r);
}
