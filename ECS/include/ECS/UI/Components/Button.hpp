/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Generic UI Button Component for ECS
*/

#pragma once

#include "../AUIComponent.hpp"
#include <string>
#include <functional>
#include <raylib.h>

// TODO: move all the implementations on the cpp file for better clarity

namespace UI {

    // Struct tio define the base colors of the buttons
    struct ButtonStyle {
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
        void handle_input() override;

        // Button-specific methods
        void set_text(const std::string& text) { _text = text; }
        const std::string& get_text() const { return _text; }

        void set_style(const ButtonStyle& style) { _style = style; }
        ButtonStyle& get_style() { return _style; }
        const ButtonStyle& get_style() const { return _style; }

        // Callbacks
        void set_on_click(std::function<void()> callback) { _on_click = std::move(callback); }
        void set_on_hover(std::function<void()> callback) { _on_hover = std::move(callback); }
        void set_on_hover_exit(std::function<void()> callback) { _on_hover_exit = std::move(callback); }
        void set_on_press(std::function<void()> callback) { _on_press = std::move(callback); }
        void set_on_release(std::function<void()> callback) { _on_release = std::move(callback); }

        // Custom rendering callback - allows game to override rendering
        void set_custom_render(std::function<void(const UIButton&)> render_func) {
            _custom_render = std::move(render_func);
        }

        // Getters for custom rendering
        UIState get_current_state() const { return _state; }

    protected:
        // Virtual methods that can be overridden for custom styling
        virtual Color get_current_background_color() const;
        virtual Color get_current_text_color() const;
        virtual void draw_button_background() const;
        virtual void draw_button_text() const;

    private:
        std::string _text;
        ButtonStyle _style;

        // Callbacks
        std::function<void()> _on_click;
        std::function<void()> _on_hover;
        std::function<void()> _on_hover_exit;
        std::function<void()> _on_press;
        std::function<void()> _on_release;
        std::function<void(const UIButton&)> _custom_render;

        // State tracking
        bool _was_hovered_last_frame{false};
        bool _was_pressed_last_frame{false};
    };
}