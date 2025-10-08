/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI Panel Component
*/

#pragma once

#include "../UIComponent.hpp"
#include <vector>
#include <memory>

class UIPanel : public UIComponent {
    private:
        Color background_color_{60, 60, 60, 200}; // Semi-transparent dark gray
        Color border_color_{100, 100, 100, 255};  // Light gray border
        bool draw_background_{true};
        bool draw_border_{true};
        float border_thickness_{2.0f};

        // Child components
        std::vector<std::shared_ptr<IUIComponent>> children_;

    public:
        UIPanel() = default;
        UIPanel(float x, float y, float width, float height)
            : UIComponent(x, y, width, height) {}

        // IUIComponent implementation
        void update(float delta_time) override;
        void render() override;
        void handle_input() override;

        // Panel-specific methods
        void set_background_color(Color color) { background_color_ = color; }
        void set_border_color(Color color) { border_color_ = color; }
        void set_draw_background(bool draw) { draw_background_ = draw; }
        void set_draw_border(bool draw) { draw_border_ = draw; }
        void set_border_thickness(float thickness) { border_thickness_ = thickness; }

        // Child management
        void add_child(std::shared_ptr<IUIComponent> child);
        void remove_child(std::shared_ptr<IUIComponent> child);
        void clear_children();
        const std::vector<std::shared_ptr<IUIComponent>>& get_children() const { return children_; }

        // Layout helpers
        void arrange_children_vertically(float spacing = 10.0f, float margin = 10.0f);
        void arrange_children_horizontally(float spacing = 10.0f, float margin = 10.0f);

    private:
        void draw_panel_background();
        void draw_panel_border();
};