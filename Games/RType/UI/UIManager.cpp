/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI Manager Implementation
*/

#include "UIManager.hpp"
#include <algorithm>
#include <iostream>

void UIManager::add_component(std::shared_ptr<IUIComponent> component) {
    if (component) {
        components_.push_back(component);
    }
}

void UIManager::add_component(const std::string& name, std::shared_ptr<IUIComponent> component) {
    if (component) {
        components_.push_back(component);
        named_components_[name] = component;
    }
}

void UIManager::remove_component(std::shared_ptr<IUIComponent> component) {
    // Remove from main vector
    components_.erase(
        std::remove(components_.begin(), components_.end(), component),
        components_.end()
    );

    // Remove from named components map
    for (auto it = named_components_.begin(); it != named_components_.end(); ) {
        if (it->second == component) {
            it = named_components_.erase(it);
        } else {
            ++it;
        }
    }
}

void UIManager::remove_component(const std::string& name) {
    auto it = named_components_.find(name);
    if (it != named_components_.end()) {
        auto component = it->second;
        named_components_.erase(it);
        remove_component(component);
    }
}

void UIManager::clear_components() {
    components_.clear();
    named_components_.clear();
}

std::shared_ptr<IUIComponent> UIManager::get_component(const std::string& name) {
    auto it = named_components_.find(name);
    return (it != named_components_.end()) ? it->second : nullptr;
}

void UIManager::update(float delta_time) {
    input_handled_this_frame_ = false;

    for (auto& component : components_) {
        if (component) {
            component->update(delta_time);
        }
    }

    cleanup_removed_components();
}

void UIManager::render() {
    for (auto& component : components_) {
        if (component) {
            component->render();
        }
    }
}

void UIManager::handle_input() {
    if (input_handled_this_frame_) return;

    // Process input for all components
    // Process in reverse order so top-most components get input first
    for (auto it = components_.rbegin(); it != components_.rend(); ++it) {
        if (*it) {
            (*it)->handle_input();
        }
    }

    input_handled_this_frame_ = true;
}

bool UIManager::is_mouse_over_ui() const {
    Vector2 mouse_pos = GetMousePosition();

    for (const auto& component : components_) {
        if (component && component->is_visible() &&
            component->is_point_inside(mouse_pos.x, mouse_pos.y)) {
            return true;
        }
    }
    return false;
}

void UIManager::set_all_visible(bool visible) {
    for (auto& component : components_) {
        if (component) {
            component->set_visible(visible);
        }
    }
}

void UIManager::set_all_enabled(bool enabled) {
    for (auto& component : components_) {
        if (component) {
            component->set_enabled(enabled);
        }
    }
}

bool UIManager::has_component(const std::string& name) const {
    return named_components_.find(name) != named_components_.end();
}

void UIManager::cleanup_removed_components() {
    // Remove any null pointers that might have been left behind
    components_.erase(
        std::remove_if(components_.begin(), components_.end(),
            [](const std::shared_ptr<IUIComponent>& ptr) { return ptr == nullptr; }),
        components_.end()
    );
}