/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** InvincibilitySys system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

#include "Entity/Components/Invincibility/Invincibility.hpp"
#include "Entity/Components/Player/Player.hpp"

class InvincibilitySys : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "InvincibilitySys"; }
private:
    void checkPlayerInvicibilty(registry &r, float dt);
    void activateInvi(Invincibility &invi, Player &player);
    void updatePlayerInvi(Invincibility &invi, float dt);
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
