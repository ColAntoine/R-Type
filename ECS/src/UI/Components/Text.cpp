/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Generic UI Text Component Implementation
*/

#include "ECS/UI/Components/Text.hpp"

namespace UI {
    void UIText::update(float delta_time) {
        if (!_visible) return;
        // Text is typically static, no update logic needed
    }

    void UIText::render() {
        if (!_visible) return;
        if (_text.empty()) return;

        // If custom render function is set, use it instead
        if (_custom_render) {
            _custom_render(*this);
            return;
        }

        // Default rendering
        if (_style._has_shadow) {
            draw_text_shadow();
        }
        draw_text_content();
    }

    void UIText::handle_input() {
        // Text typically doesn't handle input
    }

    Vector2 UIText::get_text_size() const {
        return MeasureTextEx(
            GetFontDefault(),
            _text.c_str(),
            _style._font_size,
            _style._spacing
        );
    }

    void UIText::update_size() {
        _size = get_text_size();
    }

    Vector2 UIText::calculate_text_position() const {
        Vector2 text_size = get_text_size();
        Vector2 pos = _position;

        switch (_style._alignment) {
            case TextAlignment::Center:
                pos.x -= text_size.x / 2.0f;
                break;
            case TextAlignment::Right:
                pos.x -= text_size.x;
                break;
            case TextAlignment::Left:
            default:
                // Position is already correct for left alignment
                break;
        }

        return pos;
    }

    void UIText::draw_text_shadow() const {
        if (!_style._has_shadow) return;

        Vector2 pos = calculate_text_position();
        Vector2 shadow_pos = {
            pos.x + _style._shadow_offset.x,
            pos.y + _style._shadow_offset.y
        };

        DrawTextEx(
            GetFontDefault(),
            _text.c_str(),
            shadow_pos,
            _style._font_size,
            _style._spacing,
            _style._shadow_color
        );
    }

    void UIText::draw_text_content() const {
        Vector2 pos = calculate_text_position();

        DrawTextEx(
            GetFontDefault(),
            _text.c_str(),
            pos,
            _style._font_size,
            _style._spacing,
            _style._text_color
        );
    }
}
