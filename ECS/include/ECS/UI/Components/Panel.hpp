/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Generic UI Panel Component for ECS
*/

#pragma once

#include "../AUIComponent.hpp"
#include <functional>
#include <raylib.h>

namespace UI {

    // Struct to define the base style of panels
    struct PanelStyle {
        Color _background_color{40, 40, 40, 200};  // Semi-transparent
        Color _border_color{100, 100, 100, 255};

        float _border_thickness{1.0f};
        float _corner_radius{0.0f};  // 0 for sharp corners
        bool _has_shadow{false};
        Color _shadow_color{0, 0, 0, 100};
        float _shadow_offset{5.0f};
    };

    class UIPanel : public AUIComponent {
    public:
        UIPanel() = default;

        UIPanel(float x, float y, float width, float height)
            : AUIComponent() {
            _position = {x, y};
            _size = {width, height};
        }

        // IUIComponent implementation
        void update(float delta_time) override;
        void render() override;
        void handle_input() override;

        // Panel specific methods
        void set_style(const PanelStyle& style) { _style = style; }
        PanelStyle& get_style() { return _style; }
        const PanelStyle& get_style() const { return _style; }

        // Custom rendering callback
        void set_custom_render(std::function<void(const UIPanel&)> render_func) {
            _custom_render = std::move(render_func);
        }

    protected:
        // Virtual methods that can be overridden for custom styling
        virtual void draw_panel_background() const;
        virtual void draw_panel_shadow() const;
        virtual void draw_panel_border() const;

    private:
        PanelStyle _style;
        std::function<void(const UIPanel&)> _custom_render;
    };
}
