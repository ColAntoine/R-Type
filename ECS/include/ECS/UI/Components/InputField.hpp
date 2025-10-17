/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Generic UI Input Field Component for ECS
*/

#pragma once

#include "../AUIComponent.hpp"
#include <string>
#include <functional>
#include <raylib.h>

namespace UI {

    // Struct to define the base style of input fields
    struct InputFieldStyle {
        Color _background_color{50, 50, 50, 255};
        Color _focused_color{70, 70, 70, 255};
        Color _disabled_color{30, 30, 30, 255};
        Color _border_color{100, 100, 100, 255};
        Color _focused_border_color{150, 150, 255, 255};
        Color _text_color{255, 255, 255, 255};
        Color _placeholder_color{150, 150, 150, 255};
        Color _disabled_text_color{100, 100, 100, 255};
        Color _cursor_color{255, 255, 255, 255};

        int _font_size{20};
        float _border_thickness{1.0f};
        float _padding{10.0f};
        float _cursor_blink_speed{0.5f};
    };

    class UIInputField : public AUIComponent {
    public:
        UIInputField() = default;

        UIInputField(float x, float y, float width, float height, const std::string& placeholder = "")
            : _placeholder(placeholder) {
            _position = {x, y};
            _size = {width, height};
        }

        // IUIComponent implementation
        void update(float delta_time) override;
        void render() override;
        void handle_input() override;

        // Input field specific methods
        void set_text(const std::string& text) { _text = text; }
        const std::string& get_text() const { return _text; }

        void set_placeholder(const std::string& placeholder) { _placeholder = placeholder; }
        const std::string& get_placeholder() const { return _placeholder; }

        void set_password(bool is_password) { _is_password = is_password; }
        bool is_password() const { return _is_password; }

        void set_max_length(int max_length) { _max_length = max_length; }
        int get_max_length() const { return _max_length; }

        void set_focused(bool focused) { _is_focused = focused; }
        bool is_focused() const { return _is_focused; }

        void set_style(const InputFieldStyle& style) { _style = style; }
        InputFieldStyle& get_style() { return _style; }
        const InputFieldStyle& get_style() const { return _style; }

        // Callbacks
        void set_on_text_changed(std::function<void(const std::string&)> callback) {
            _on_text_changed = std::move(callback);
        }

        void set_on_enter_pressed(std::function<void(const std::string&)> callback) {
            _on_enter_pressed = std::move(callback);
        }

        void set_on_focus(std::function<void()> callback) {
            _on_focus = std::move(callback);
        }

        void set_on_focus_lost(std::function<void()> callback) {
            _on_focus_lost = std::move(callback);
        }

        // Custom rendering callback
        void set_custom_render(std::function<void(const UIInputField&)> render_func) {
            _custom_render = std::move(render_func);
        }

    protected:
        // Virtual methods that can be overridden for custom styling
        virtual Color get_current_background_color() const;
        virtual Color get_current_border_color() const;
        virtual Color get_current_text_color() const;
        virtual void draw_input_background() const;
        virtual void draw_input_text() const;
        virtual void draw_cursor() const;
        virtual std::string get_display_text() const;

    private:
        void process_text_input();

        std::string _text;
        std::string _placeholder;
        InputFieldStyle _style;

        bool _is_focused{false};
        bool _is_password{false};
        int _max_length{50};

        float _cursor_blink_timer{0.0f};
        bool _show_cursor{true};

        // Callbacks
        std::function<void(const std::string&)> _on_text_changed;
        std::function<void(const std::string&)> _on_enter_pressed;
        std::function<void()> _on_focus;
        std::function<void()> _on_focus_lost;
        std::function<void(const UIInputField&)> _custom_render;

        // State tracking
        bool _was_focused_last_frame{false};
    };
}
