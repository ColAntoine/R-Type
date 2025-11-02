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
#include "Components/Controllable/Controllable.hpp"
#include "Components/Player/Player.hpp"

#include "ECS/Registry.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Components.hpp"

#include <functional>

// Callback type for broadcasting entity destruction to clients
using EntityDestroyCallback = std::function<void(entity, uint8_t reason)>;

class HealthSys : public ISystem {
public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "Health"; }
    
    void set_destroy_callback(EntityDestroyCallback callback) {
        destroy_callback_ = std::move(callback);
    }

private:
    void checkAndKillEnemy(registry &r);
    void checkAndKillPlayer(registry &r);
    void addScore(registry &r, int amount = 1);
    void emitPlayerHealthStats(registry &r);
    
    EntityDestroyCallback destroy_callback_;
};

// Global function to set entity destroy callback (for server broadcasting)
void set_global_entity_destroy_callback(EntityDestroyCallback callback);


#if defined(_WIN32)
  #define DLL_EXPORT extern "C" __declspec(dllexport)
#else
  #define DLL_EXPORT extern "C"
#endif

DLL_EXPORT ISystem* create_system();
DLL_EXPORT void     destroy_system(ISystem* ptr);