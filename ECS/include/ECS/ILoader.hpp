/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ILoader
*/

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <filesystem>

#include "ComponentFactory.hpp"
#include "ECS/Systems/ISystem.hpp"


class registry;

class ILoader {
    public:
        enum SystemType {
            LogicSystem,
            RenderSystem
        };
        virtual ~ILoader() = default;

        // Components
        virtual bool load_components(const std::string& so_path, registry& reg) = 0;
        virtual IComponentFactory* get_factory() const = 0;

        // Systems
        virtual bool load_system(const std::string& so_path, SystemType type) = 0;
        virtual void update_all_systems(registry& r, float dt, SystemType type) = 0;
        virtual void update_system_by_name(const std::string& name, registry& r, float dt, SystemType type) = 0;

        // Status
        virtual bool is_loaded() const = 0;
        virtual size_t get_system_count(SystemType type) const = 0;
        virtual std::vector<std::string> get_system_names(SystemType typ) const = 0;
};