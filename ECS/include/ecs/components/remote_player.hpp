/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Remote Player Component
*/

#pragma once

#include <string>
#include "ecs/icomponent.hpp"

struct remote_player : public IComponent {
    std::string client_id;

    remote_player();
    remote_player(const std::string& id);
};
