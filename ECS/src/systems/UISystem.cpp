/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI System Implementation using Registry and Zipper
*/

#include "ECS/Systems/UISystem.hpp"
#include "ECS/Components/UIComponent.hpp"
#include "ECS/Zipper.hpp"
#include <iostream>

namespace UI {
    void UISystem::update(registry& registry, float deltaTime) {
        if (!_enabled) return;

        // Get UI components array
        auto* ui_components = registry.get_if<UI::UIComponent>();
        if (!ui_components)
            return;
        // Use zipper to iterate through UI components with their entity IDs
        for (auto [ui_comp, entity_id] : zipper(*ui_components)) {
            if (ui_comp._ui_element) {
                ui_comp._ui_element->update(deltaTime);
            }
        }
        this->process_input(registry);
        this->render(registry);
    }

    void UISystem::process_input(registry& registry) {
        if (!_enabled) return;

        handle_mouse_input(registry);
        handle_keyboard_input(registry);
    }

    void UISystem::render(registry& registry) {
        if (!_enabled) return;

        // Get UI components array
        auto* ui_components = registry.get_if<UI::UIComponent>();
        if (!ui_components) return;

        for (auto [ui_comp, entity_id] : zipper(*ui_components)) {
            if (ui_comp._ui_element) {
                ui_comp._ui_element->render();
            }
        }
    }

    void UISystem::handle_mouse_input(registry& registry) {
        Vector2 mouse_pos = GetMousePosition();
        auto* ui_components = registry.get_if<UI::UIComponent>();
        if (!ui_components) return;

        // Handle input for all UI components using zipper
        for (auto [ui_comp, entity_id] : zipper(*ui_components)) {
            if (ui_comp._ui_element &&
                ui_comp._ui_element->isVisible() &&
                ui_comp._ui_element->isEnabled()) {
                ui_comp._ui_element->handleInput();
            }
        }

        // Update focus on click
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            size_t clicked_entity = get_entity_at_position(registry, mouse_pos.x, mouse_pos.y);
            if (clicked_entity != static_cast<size_t>(-1)) {
                set_focused_entity(clicked_entity);
            } else {
                clear_focus();
            }
        }
    }

    void UISystem::handle_keyboard_input(registry& registry) {
        if (!has_focus()) return;

        auto* ui_components = registry.get_if<UI::UIComponent>();
        if (!ui_components) return;

        // Find and handle input for focused component using zipper
        for (auto [ui_comp, entity_id] : zipper(*ui_components)) {
            if (entity_id == _focused_entity_id &&
                ui_comp._ui_element &&
                ui_comp._ui_element->isEnabled()) {
                // Keyboard handling for focused element
                // This can be extended based on component type (e.g., text input)
                break;
            }
        }
    }

    size_t UISystem::get_entity_at_position(registry& registry, float x, float y) {
        auto* ui_components = registry.get_if<UI::UIComponent>();
        if (!ui_components) return static_cast<size_t>(-1);

        // Store all clickable entities with their z-index
        std::vector<std::pair<size_t, int>> clickable_entities;

        // Use zipper to find all components under the cursor
        for (auto [ui_comp, entity_id] : zipper(*ui_components)) {
            if (ui_comp._ui_element &&
                ui_comp._ui_element->isVisible() &&
                ui_comp._ui_element->isEnabled() &&
                ui_comp._ui_element->isPointInside(x, y)) {
                clickable_entities.push_back({entity_id, 0});
            }
        }

        // Return the last entity found (top-most in z-order)
        if (!clickable_entities.empty()) {
            return clickable_entities.back().first;
        }

        return static_cast<size_t>(-1); // No entity found
    }
} // namespace UI

// Factory function for dynamic loading (outside namespace)
DLL_EXPORT ISystem* create_system() {
    try {
        return new UI::UISystem();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}