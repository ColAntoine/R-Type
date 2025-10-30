/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Health system
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

#include "Components/Health/Health.hpp"
#include "Components/Score/Score.hpp"

#include "ECS/Registry.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Components.hpp"

class HealthSys : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "Health"; }
private:
    void checkAndKillEnemy(registry &r);
    void checkAndKillPlayer(registry &r);
    void addScore(registry &r);
};


#if defined(_WIN32)
  #define DLL_EXPORT extern "C" __declspec(dllexport)
#else
  #define DLL_EXPORT extern "C"
#endif

DLL_EXPORT ISystem* create_system();
DLL_EXPORT void     destroy_system(ISystem* ptr);