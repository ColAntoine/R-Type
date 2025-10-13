/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Enemy AI System
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Components.hpp"
#include "Entity/Components/Enemy/Enemy.hpp"

class EnemyAISystem : public ISystem {
private:
    float world_height_{768.0f};

public:
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "EnemyAISystem"; }

    void set_world_height(float height) { world_height_ = height; }
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}