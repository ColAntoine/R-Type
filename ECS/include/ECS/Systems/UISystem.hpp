/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI System for managing UI components via Registry
*/

#pragma once

#include "ISystem.hpp"
#include "../Components/UIComponent.hpp"
#include "../Registry.hpp"
#include "../UI/IUIComponent.hpp"
#include <raylib.h>

namespace UI {
    class UISystem : public ISystem {
    public:
        UISystem() = default;
        virtual ~UISystem() = default;

        // ISystem implementation
        void update(registry& registry, float deltaTime) override;
        const char* get_name() const override { return "UISystem"; }

        // UI-specific methods
        void process_input(registry& registry);
        void render(registry& registry);

        // Focus management
        void set_focused_entity(size_t entity_id) { _focused_entity_id = entity_id; }
        size_t get_focused_entity() const { return _focused_entity_id; }
        void clear_focus() { _focused_entity_id = static_cast<size_t>(-1); }
        bool has_focus() const { return _focused_entity_id != static_cast<size_t>(-1); }

        // Enable/disable entire UI system
        void set_enabled(bool enabled) { _enabled = enabled; }
        bool is_enabled() const { return _enabled; }

    private:
        size_t _focused_entity_id{static_cast<size_t>(-1)};
        bool _enabled{true};

        // Helper methods
        size_t get_entity_at_position(registry& registry, float x, float y);
        void handle_mouse_input(registry& registry);
        void handle_keyboard_input(registry& registry);
    };
}

#if defined(_WIN32)
  #define DLL_EXPORT extern "C" __declspec(dllexport)
#else
  #define DLL_EXPORT extern "C"
#endif

DLL_EXPORT ISystem* create_system();
DLL_EXPORT void     destroy_system(ISystem* ptr);