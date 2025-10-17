/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Generic UI Button Component Implementation
*/

#include "ECS/UI/Components/Button.hpp"

namespace UI {
    void UIButton::update(float delta_time) {
        if (!_visible) return;

        UIState previous_state = _state;
        update_state();

        // Track state changes for callbacks
        bool is_hovered = (_state == UIState::Hovered || _state == UIState::Pressed);
        bool is_pressed = (_state == UIState::Pressed);

        // Hover callbacks
        if (is_hovered && !_was_hovered_last_frame && _on_hover) {
            _on_hover();
        } else if (!is_hovered && _was_hovered_last_frame && _on_hover_exit) {
            _on_hover_exit();
        }

        // Press/Release callbacks
        if (is_pressed && !_was_pressed_last_frame && _on_press) {
            _on_press();
        } else if (!is_pressed && _was_pressed_last_frame && _on_release) {
            _on_release();
        }

        _was_hovered_last_frame = is_hovered;
        _was_pressed_last_frame = is_pressed;
    }

    void UIButton::render() {
        if (!_visible) return;

        // If custom render function is set, use it instead
        if (_custom_render) {
            _custom_render(*this);
            return;
        }

        // Default rendering
        draw_button_background();
        draw_button_text();
    }

    void UIButton::handle_input() {
        if (!_visible || !_enabled) return;

        Vector2 mouse_pos = GetMousePosition();
        bool is_hovered = is_point_inside(mouse_pos.x, mouse_pos.y);
        bool is_clicked = is_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (is_clicked && _on_click) {
            _on_click();
        }
    }

    Color UIButton::get_current_background_color() const {
        switch (_state) {
            case UIState::Hovered:
                return _style._hovered_color;
            case UIState::Pressed:
                return _style._pressed_color;
            case UIState::Disabled:
                return _style._disabled_color;
            case UIState::Normal:
            default:
                return _style._normal_color;
        }
    }

    Color UIButton::get_current_text_color() const {
        return (_state == UIState::Disabled) ? _style._disabled_text_color : _style._text_color;
    }

    void UIButton::draw_button_background() const {
        Color bg_color = get_current_background_color();

        // Draw main button rectangle
        DrawRectangle(_position.x, _position.y, _size.x, _size.y, bg_color);

        // Draw border
        DrawRectangleLinesEx(
            {_position.x, _position.y, _size.x, _size.y},
            _style._border_thickness,
            _style._border_color
        );
    }

    void UIButton::draw_button_text() const {
        if (_text.empty()) return;

        Color text_color = get_current_text_color();

        // Calculate text position to center it in the button
        Vector2 textSize = MeasureTextEx(GetFontDefault(), _text.c_str(), _style._font_size, 1.0f);
        float textX = _position.x + (_size.x - textSize.x) / 2.0f;
        float textY = _position.y + (_size.y - textSize.y) / 2.0f;

        DrawTextEx(GetFontDefault(), _text.c_str(), {textX, textY}, _style._font_size, 1.0f, text_color);
    }
}