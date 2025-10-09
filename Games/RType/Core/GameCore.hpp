/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GameCore - Shared base for Client and Server
*/

#pragma once

#include "ECS/Registry.hpp"
#include "ECS/DLLoader.hpp"
#include "ECS/ComponentFactory.hpp"
#include <memory>
#include <string>

/**
 * @brief Base class containing shared ECS logic
 * 
 * Provides:
 * - ECS registry management
 * - Component factory
 * - System loading infrastructure
 * - Common game constants
 */
class GameCore {
protected:
    // ECS
    registry ecs_registry_;
    DLLoader system_loader_;
    IComponentFactory* component_factory_{nullptr};

    // Game constants
    float world_width_{1920.0f};
    float world_height_{1080.0f};
    float fixed_timestep_{1.0f / 60.0f};

    bool initialized_{false};

public:
    GameCore() = default;
    virtual ~GameCore() = default;

    // Getters
    registry& get_registry() { return ecs_registry_; }
    IComponentFactory* get_factory() { return component_factory_; }

protected:
    /**
     * @brief Setup ECS base (load components)
     */
    void setup_ecs_base();

    /**
     * @brief Load shared systems (physics, collision, AI)
     */
    void load_shared_systems();

    /**
     * @brief Pure virtual - subclasses implement specific system loading
     */
    virtual void load_specific_systems() = 0;
};