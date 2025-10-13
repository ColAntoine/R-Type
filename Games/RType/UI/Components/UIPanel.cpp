/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI Panel Component Implementation
*/

#include "UIPanel.hpp"
#include <algorithm>

void UIPanel::update(float delta_time) {
    if (!visible_) return;

    // Update all child components
    for (auto& child : children_) {
        if (child) {
            child->update(delta_time);
        }
    }
}

void UIPanel::render() {
    if (!visible_) return;

    // Draw panel background and border
    if (draw_background_) {
        draw_panel_background();
    }
    if (draw_border_) {
        draw_panel_border();
    }

    // Render all child components
    for (auto& child : children_) {
        if (child) {
            child->render();
        }
    }
}

void UIPanel::handle_input() {
    if (!visible_ || !enabled_) return;

    // Handle input for all child components
    // Process in reverse order so top-most children get input first
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        if (*it) {
            (*it)->handle_input();
        }
    }
}

void UIPanel::add_child(std::shared_ptr<IUIComponent> child) {
    if (child) {
        children_.push_back(child);
    }
}

void UIPanel::remove_child(std::shared_ptr<IUIComponent> child) {
    children_.erase(
        std::remove(children_.begin(), children_.end(), child),
        children_.end()
    );
}

void UIPanel::clear_children() {
    children_.clear();
}

void UIPanel::arrange_children_vertically(float spacing, float margin) {
    float current_y = position_.y + margin;

    for (auto& child : children_) {
        if (child && child->is_visible()) {
            // Vector2 child_pos = child->get_position();
            child->set_position(position_.x + margin, current_y);
            current_y += child->get_size().y + spacing;
        }
    }
}

void UIPanel::arrange_children_horizontally(float spacing, float margin) {
    float current_x = position_.x + margin;

    for (auto& child : children_) {
        if (child && child->is_visible()) {
            // Vector2 child_pos = child->get_position();
            child->set_position(current_x, position_.y + margin);
            current_x += child->get_size().x + spacing;
        }
    }
}

void UIPanel::draw_panel_background() {
    DrawRectangle(position_.x, position_.y, size_.x, size_.y, background_color_);
}

void UIPanel::draw_panel_border() {
    DrawRectangleLinesEx(
        {position_.x, position_.y, size_.x, size_.y},
        border_thickness_,
        border_color_
    );
}