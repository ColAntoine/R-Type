/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ALoader
*/

#pragma once

#include "ILoader.hpp"

#include <exception>
#include <iostream>

#ifdef _WIN32
    // Prevent Windows API conflicts with Raylib
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    // Undefine conflicting macros after windows.h is included
    #include <windows.h>
    #undef Rectangle
    #undef CloseWindow
    #undef ShowCursor
    #undef DrawText
    #undef PlaySound
#endif

class ALoader : public ILoader{
    public:
        ALoader() = default;
        ~ALoader() override = default;

        ALoader(const ALoader&) = delete;
        ALoader& operator=(const ALoader&) = delete;
        ALoader(ALoader&& other) noexcept;
        ALoader& operator=(ALoader&& other) noexcept;

        //Need to implemented in every different loader
        bool load_components(const std::string& so_path, registry& reg) override = 0;
        bool load_system(const std::string& so_path) override = 0;
        
        
        
        IComponentFactory* get_factory() const override;
        void update_all_systems(registry& r, float dt) override;
        void update_system_by_name(const std::string& name, registry& r, float dt) override;
        bool is_loaded() const override;
        size_t get_system_count() const override;
        std::vector<std::string> get_system_names() const override;

    protected:
        struct LoadedSystem {
            #ifdef _WIN32
                    HMODULE handle; //Windows
            #else
                    void* handle; //linux and macos
            #endif
            std::unique_ptr<ISystem> system;
            std::string name;
        };

        #ifdef _WIN32
            HMODULE library_handle_ = nullptr;
        #else
            void* library_handle_ = nullptr;
        #endif
        IComponentFactory* factory_ = nullptr;     // For components
        std::vector<LoadedSystem> systems_; // For systems
    
};

