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
        public:
        // Getters
            Color getBackgroundColor() const { return _background_color; }
            Color getBorderColor() const { return _border_color; }
            float getBorderThickness() const { return _border_thickness; }
            float getCornerRadius() const { return _corner_radius; }
            bool hasShadow() const { return _has_shadow; }
            Color getShadowColor() const { return _shadow_color; }
            float getShadowOffset() const { return _shadow_offset; }

            // Setters
            void setBackgroundColor(const Color& color) { _background_color = color; }
            void setBorderColor(const Color& color) { _border_color = color; }
            void setBorderThickness(float thickness) { _border_thickness = thickness; }
            void setCornerRadius(float radius) { _corner_radius = radius; }
            void setHasShadow(bool shadow) { _has_shadow = shadow; }
            void setShadowColor(const Color& color) { _shadow_color = color; }
            void setShadowOffset(float offset) { _shadow_offset = offset; }

        private:
            Color _background_color{40, 40, 40, 200};
            Color _border_color{100, 100, 100, 255};

            float _border_thickness{1.0f};
            float _corner_radius{0.0f};
            bool _has_shadow{false};
            Color _shadow_color{0, 0, 0, 100};
            float _shadow_offset{5.0f};
    };

    class UIPanel : public AUIComponent {
    public:
        UIPanel() = default;
        PanelStyle _style;

        UIPanel(float x, float y, float width, float height)
            : AUIComponent() {
            _position = {x, y};
            _size = {width, height};
        }

        // IUIComponent implementation
        void update(float delta_time) override;
        void render() override;
        void handleInput() override;

        // Panel specific methods
        void setStyle(const PanelStyle& style) { _style = style; }
        PanelStyle& getStyle() { return _style; }
        const PanelStyle& getStyle() const { return _style; }

        // Custom rendering callback
        void setCustomRender(std::function<void(const UIPanel&)> render_func) {
            _customRender = std::move(render_func);
        }

    protected:
        // Virtual methods that can be overridden for custom styling
        virtual void drawPanelBackground() const;
        virtual void drawPanelShadow() const;
        virtual void drawPanelBorder() const;

    private:
        std::function<void(const UIPanel&)> _customRender;
    };
}
