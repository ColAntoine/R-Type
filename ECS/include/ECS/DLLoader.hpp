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

#include "ComponentFactory.hpp"
#include "ECS/Systems/ISystem.hpp"

class registry;

class DLLoader {
    public:
        enum SystemType {
            LogicSystem,
            RenderSystem
        };
        DLLoader();
        ~DLLoader();

        // Component loading (existing functionality)
        bool load_components_from_so(const std::string &so_path, registry &reg);
        IComponentFactory* get_factory() const;

        // System loading (new functionality)
        bool load_system_from_so(const std::string &so_path, SystemType type);
        void update_all_systems(registry& r, float dt, SystemType type);
        void update_system_by_name(const std::string& name, registry& r, float dt, SystemType type);

        // General status
        bool is_loaded() const;
        size_t get_system_count(SystemType type) const;
        std::vector<std::string> get_system_names(SystemType type) const;

    private:
        struct LoadedSystem {
            void* handle;
            std::unique_ptr<ISystem> system;
            std::string name;
        };

        void* library_handle_;           // For components
        IComponentFactory* factory_;     // For components
        std::vector<LoadedSystem> _logicSystems; // For logic systems
        std::vector<LoadedSystem> _renderSystems; // For render systems

        // Non-copyable, movable
        DLLoader(const DLLoader&) = delete;
        DLLoader& operator=(const DLLoader&) = delete;
        DLLoader(DLLoader&& other) noexcept;
        DLLoader& operator=(DLLoader&& other) noexcept;
};