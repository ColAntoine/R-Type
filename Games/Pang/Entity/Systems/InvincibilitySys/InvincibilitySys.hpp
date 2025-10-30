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
    bool activateInvi(registry &r, Invincibility &invi, Player &player, std::size_t ent);
    void updatePlayerInvi(Invincibility &invi, float dt);
};

#if defined(_WIN32)
  #define DLL_EXPORT extern "C" __declspec(dllexport)
#else
  #define DLL_EXPORT extern "C"
#endif

DLL_EXPORT ISystem* create_system();
DLL_EXPORT void     destroy_system(ISystem* ptr);