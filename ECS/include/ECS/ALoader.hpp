/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ALoader
*/

#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <filesystem>

#include "ILoader.hpp"

class ALoader : public ILoader {
    public:
        ALoader() = default;
        ~ALoader() override = default;

        ALoader(const ALoader&) = delete;
        ALoader& operator=(const ALoader&) = delete;
        ALoader(ALoader&& other) noexcept;
        ALoader& operator=(ALoader&& other) noexcept;

        //Need to implemented in every different loader
        // virtual bool load_components(const std::string& so_path, registry& reg) override;
        // virtual bool load_system(const std::string& so_path, SystemType type) override;
        
        
        
        IComponentFactory* get_factory() const override;
        void update_all_systems(registry& r, float dt, SystemType type) override;
        void update_system_by_name(const std::string& name, registry& r, float dt, SystemType type) override;
        bool is_loaded() const override;
        size_t get_system_count(SystemType type) const override;
        std::vector<std::string> get_system_names(SystemType typ) const override;

    protected:
        struct LoadedSystem {
            void* handle =  nullptr;
            std::unique_ptr<ISystem, std::function<void(ISystem*)>> system;
            std::string name;
        };
        void* library_handle_ = nullptr;
        IComponentFactory* factory_ = nullptr;     // For components
        std::vector<LoadedSystem> _logicSystems; // For logic systems
        std::vector<LoadedSystem> _renderSystems; // For render systems
};
