/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Generic UI Button Component for ECS
*/

#pragma once

#include "ECS/UI/AUIComponent.hpp"
#include <string>
#include <functional>
#include <raylib.h>

namespace UI {

    // Struct tio define the base colors of the buttons
    struct ButtonStyle {
    public:
        // Getters
        Color getNormalColor() const { return _normal_color; }
        Color getHoveredColor() const { return _hovered_color; }
        Color getPressedColor() const { return _pressed_color; }
        Color getDisabledColor() const { return _disabled_color; }
        Color getTextColor() const { return _text_color; }
        Color getDisabledTextColor() const { return _disabled_text_color; }
        Color getBorderColor() const { return _border_color; }
        int getFontSize() const { return _font_size; }
        float getBorderThickness() const { return _border_thickness; }
        float getPadding() const { return _padding; }

        // Setters
        void setNormalColor(const Color& color) { _normal_color = color; }
        void setHoveredColor(const Color& color) { _hovered_color = color; }
        void setPressedColor(const Color& color) { _pressed_color = color; }
        void setDisabledColor(const Color& color) { _disabled_color = color; }
        void setTextColor(const Color& color) { _text_color = color; }
        void setDisabledTextColor(const Color& color) { _disabled_text_color = color; }
        void setBorderColor(const Color& color) { _border_color = color; }
        void setFontSize(int size) { _font_size = size; }
        void setBorderThickness(float thickness) { _border_thickness = thickness; }
        void setPadding(float padding) { _padding = padding; }

    private:
        Color _normal_color{70, 70, 70, 255};
        Color _hovered_color{100, 100, 100, 255};
        Color _pressed_color{50, 50, 50, 255};
        Color _disabled_color{40, 40, 40, 255};
        Color _text_color{255, 255, 255, 255};
        Color _disabled_text_color{150, 150, 150, 255};
        Color _border_color{100, 100, 100, 255};

        int _font_size{20};
        float _border_thickness{1.0f};
        float _padding{10.0f};
    };

    class UIButton : public AUIComponent {
    public:
        UIButton() = default;

        UIButton(float x, float y, float width, float height, const std::string& text)
            : _text(text) {
            _position = {x, y};
            _size = {width, height};
        }

        // IUIComponent implementation
        void update(float delta_time) override;
        void render() override;
        void handleInput() override;

        // Button-specific methods
        std::string getText() { return _text; }
        void setText(const std::string& text) { _text = text; }
        const std::string& getText() const { return _text; }

        void setStyle(const ButtonStyle& style) { _style = style; }
        ButtonStyle& getStyle() { return _style; }
        const ButtonStyle& getStyle() const { return _style; }

        // Callbacks
        void setOnClick(std::function<void()> callback) { _on_click = std::move(callback); }
        void setOnHover(std::function<void()> callback) { _on_hover = std::move(callback); }
        void setOnHoverExit(std::function<void()> callback) { _on_hover_exit = std::move(callback); }
        void setOnPress(std::function<void()> callback) { _on_press = std::move(callback); }
        void setOnRelease(std::function<void()> callback) { _on_release = std::move(callback); }

        // Custom rendering callback - allows game to override rendering
        void setCustomRender(std::function<void(const UIButton&)> render_func) {
            _customRender = std::move(render_func);
        }

        // Getters for custom rendering
        UIState getCurrentState() const { return _state; }

    protected:
        virtual Color getCurrentBackgroundColor() const;
        virtual Color getCurrentTextColor() const;
        virtual void drawButtonBackground() const;
        virtual void drawButtonText() const;

    public:
        ButtonStyle _style;

    private:
        std::string _text;

        // Callbacks
        std::function<void()> _on_click;
        std::function<void()> _on_hover;
        std::function<void()> _on_hover_exit;
        std::function<void()> _on_press;
        std::function<void()> _on_release;
        std::function<void(const UIButton&)> _customRender;

        // State tracking
        bool _was_hovered_last_frame{false};
        bool _was_pressed_last_frame{false};
    };
}