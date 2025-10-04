/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI Manager - Coordinates all UI components
*/

#pragma once

#include "ui_component.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

class UIManager {
    private:
        std::vector<std::shared_ptr<IUIComponent>> components_;
        std::unordered_map<std::string, std::shared_ptr<IUIComponent>> named_components_;
        bool input_handled_this_frame_{false};

    public:
        UIManager() = default;
        ~UIManager() = default;

        // Component management
        void add_component(std::shared_ptr<IUIComponent> component);
        void add_component(const std::string& name, std::shared_ptr<IUIComponent> component);
        void remove_component(std::shared_ptr<IUIComponent> component);
        void remove_component(const std::string& name);
        void clear_components();

        // Component retrieval
        std::shared_ptr<IUIComponent> get_component(const std::string& name);
        template<typename T>
        std::shared_ptr<T> get_component(const std::string& name) {
            auto component = get_component(name);
            return std::dynamic_pointer_cast<T>(component);
        }

        // UI System methods
        void update(float delta_time);
        void render();
        void handle_input();

        // Utility methods
        bool is_mouse_over_ui() const;
        void set_all_visible(bool visible);
        void set_all_enabled(bool enabled);

        // Component count
        size_t get_component_count() const { return components_.size(); }
        bool has_component(const std::string& name) const;

    private:
        void cleanup_removed_components();
};