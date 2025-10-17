/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Generic UI Panel Component Implementation
*/

#include "ECS/UI/Components/Panel.hpp"

namespace UI {
    void UIPanel::update(float delta_time) {
        if (!_visible) return;
        // Panels are typically static, no update logic needed
    }

    void UIPanel::render() {
        if (!_visible) return;

        // If custom render function is set, use it instead
        if (_custom_render) {
            _custom_render(*this);
            return;
        }

        // Default rendering
        if (_style._has_shadow) {
            draw_panel_shadow();
        }
        draw_panel_background();
        draw_panel_border();
    }

    void UIPanel::handle_input() {}

    void UIPanel::draw_panel_shadow() const {
        if (!_style._has_shadow) return;

        DrawRectangle(
            _position.x + _style._shadow_offset,
            _position.y + _style._shadow_offset,
            _size.x,
            _size.y,
            _style._shadow_color
        );
    }

    void UIPanel::draw_panel_background() const {
        if (_style._corner_radius > 0) {
            // Draw rounded rectangle
            DrawRectangleRounded(
                {_position.x, _position.y, _size.x, _size.y},
                _style._corner_radius / _size.y,  // Normalize radius
                8,  // Segments for rounding
                _style._background_color
            );
        } else {
            // Draw regular rectangle
            DrawRectangle(
                _position.x,
                _position.y,
                _size.x,
                _size.y,
                _style._background_color
            );
        }
    }

    void UIPanel::draw_panel_border() const {
        if (_style._border_thickness <= 0) return;

        if (_style._corner_radius > 0) {
            // Draw rounded rectangle border
            DrawRectangleRoundedLines(
                {_position.x, _position.y, _size.x, _size.y},
                _style._corner_radius / _size.y,
                8,
                _style._border_color
            );
        } else {
            // Draw regular rectangle border
            DrawRectangleLinesEx(
                {_position.x, _position.y, _size.x, _size.y},
                _style._border_thickness,
                _style._border_color
            );
        }
    }
}
