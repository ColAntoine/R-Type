/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ParabolSys system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Zipper.hpp"

#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"

#include "Entity/Components/Gravity/Gravity.hpp"
#include "Entity/Components/Parabol/Parabol.hpp"

class ParabolSys : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "ParabolSys"; }

private:
    void handleParabols(registry &r);
};


#if defined(_WIN32)
  #define DLL_EXPORT extern "C" __declspec(dllexport)
#else
  #define DLL_EXPORT extern "C"
#endif

DLL_EXPORT ISystem* create_system();
DLL_EXPORT void     destroy_system(ISystem* ptr);
