/*
** EPITECH PROJECT, 2025
** asd
** File description:
** asd
*/

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "component_factory.hpp"
#include "ecs/systems/isystem.hpp"

class registry;

class DLLoader {
    public:
        DLLoader();
        ~DLLoader();

        // Component loading (existing functionality)
        bool load_components_from_so(const std::string &so_path, registry &reg);
        IComponentFactory* get_factory() const;

        // System loading (new functionality)
        bool load_system_from_so(const std::string &so_path);
        void update_all_systems(registry& r, float dt);
        void update_system_by_name(const std::string& name, registry& r, float dt);

        // General status
        bool is_loaded() const;
        size_t get_system_count() const;
        std::vector<std::string> get_system_names() const;

    private:
        struct LoadedSystem {
            void* handle;
            std::unique_ptr<ISystem> system;
            std::string name;
        };

        void* library_handle_;           // For components
        IComponentFactory* factory_;     // For components
        std::vector<LoadedSystem> systems_; // For systems

        // Non-copyable, movable
        DLLoader(const DLLoader&) = delete;
        DLLoader& operator=(const DLLoader&) = delete;
        DLLoader(DLLoader&& other) noexcept;
        DLLoader& operator=(DLLoader&& other) noexcept;
};