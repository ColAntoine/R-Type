/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Remote Player Component
*/

#pragma once

#include <string>
#include "ECS/Components/IComponent.hpp"

struct remote_player : public IComponent {
    std::string client_id;

    remote_player();
    remote_player(const std::string& id);
};
