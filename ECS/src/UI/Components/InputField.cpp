/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Generic UI Input Field Component Implementation
*/

#include "ECS/UI/Components/InputField.hpp"

namespace UI {
    void UIInputField::update(float delta_time) {
        if (!_visible) return;

        // Update cursor blink timer
        if (_is_focused) {
            _cursor_blink_timer += delta_time;
            if (_cursor_blink_timer >= _style._cursor_blink_speed) {
                _cursor_blink_timer = 0.0f;
                _show_cursor = !_show_cursor;
            }
        }

        // Handle focus state changes
        if (_is_focused && !_was_focused_last_frame && _on_focus) {
            _on_focus();
        } else if (!_is_focused && _was_focused_last_frame && _on_focus_lost) {
            _on_focus_lost();
        }

        _was_focused_last_frame = _is_focused;
    }

    void UIInputField::render() {
        if (!_visible) return;

        // If custom render function is set, use it instead
        if (_custom_render) {
            _custom_render(*this);
            return;
        }

        // Default rendering
        draw_input_background();
        draw_input_text();

        if (_is_focused && _show_cursor) {
            draw_cursor();
        }
    }

    void UIInputField::handle_input() {
        if (!_visible || !_enabled) return;

        Vector2 mouse_pos = GetMousePosition();
        bool is_hovered = is_point_inside(mouse_pos.x, mouse_pos.y);

        // Handle focus on click
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            _is_focused = is_hovered;
        }

        // Process text input if focused
        if (_is_focused) {
            process_text_input();
        }
    }

    void UIInputField::process_text_input() {
        // Handle backspace
        if (IsKeyPressed(KEY_BACKSPACE) && !_text.empty()) {
            _text.pop_back();
            if (_on_text_changed) {
                _on_text_changed(_text);
            }
        }

        // Handle enter key
        if (IsKeyPressed(KEY_ENTER) && _on_enter_pressed) {
            _on_enter_pressed(_text);
        }

        // Handle text input
        int key = GetCharPressed();
        while (key > 0) {
            // Only allow printable characters
            if ((key >= 32) && (key <= 126) && (_text.length() < static_cast<size_t>(_max_length))) {
                _text += static_cast<char>(key);
                if (_on_text_changed) {
                    _on_text_changed(_text);
                }
            }
            key = GetCharPressed();
        }
    }

    Color UIInputField::get_current_background_color() const {
        if (!_enabled) {
            return _style._disabled_color;
        }
        return _is_focused ? _style._focused_color : _style._background_color;
    }

    Color UIInputField::get_current_border_color() const {
        if (!_enabled) {
            return _style._border_color;
        }
        return _is_focused ? _style._focused_border_color : _style._border_color;
    }

    Color UIInputField::get_current_text_color() const {
        return _enabled ? _style._text_color : _style._disabled_text_color;
    }

    void UIInputField::draw_input_background() const {
        Color bg_color = get_current_background_color();
        Color border_color = get_current_border_color();

        // Draw background
        DrawRectangle(_position.x, _position.y, _size.x, _size.y, bg_color);

        // Draw border
        DrawRectangleLinesEx(
            {_position.x, _position.y, _size.x, _size.y},
            _style._border_thickness,
            border_color
        );
    }

    void UIInputField::draw_input_text() const {
        std::string display_text = get_display_text();
        Color text_color = get_current_text_color();

        // Show placeholder if empty
        if (display_text.empty() && !_placeholder.empty()) {
            display_text = _placeholder;
            text_color = _style._placeholder_color;
        }

        if (display_text.empty()) return;

        // Calculate text position with padding
        float text_x = _position.x + _style._padding;
        float text_y = _position.y + (_size.y - _style._font_size) / 2.0f;

        DrawTextEx(
            GetFontDefault(),
            display_text.c_str(),
            {text_x, text_y},
            _style._font_size,
            1.0f,
            text_color
        );
    }

    void UIInputField::draw_cursor() const {
        std::string display_text = get_display_text();

        // Calculate cursor position
        float text_x = _position.x + _style._padding;
        float text_y = _position.y + (_size.y - _style._font_size) / 2.0f;

        Vector2 text_size = MeasureTextEx(
            GetFontDefault(),
            display_text.c_str(),
            _style._font_size,
            1.0f
        );

        float cursor_x = text_x + text_size.x + 2.0f;
        float cursor_height = _style._font_size;

        // Draw cursor
        DrawRectangle(
            cursor_x,
            text_y,
            2.0f,
            cursor_height,
            _style._cursor_color
        );
    }

    std::string UIInputField::get_display_text() const {
        if (_is_password && !_text.empty()) {
            return std::string(_text.length(), '*');
        }
        return _text;
    }
}