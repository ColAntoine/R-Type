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

        if (_customRender) {
            _customRender(*this);
            return;
        }

        // Default rendering
        if (_style.hasShadow()) {
            drawPanelShadow();
        }
        drawPanelBackground();
        drawPanelBorder();
    }

    void UIPanel::handleInput() {}

    void UIPanel::drawPanelShadow() const {
        if (!_style.hasShadow()) return;

        DrawRectangle(
            _position.x + _style.getShadowOffset(),
            _position.y + _style.getShadowOffset(),
            _size.x,
            _size.y,
            _style.getShadowColor()
        );
    }

    void UIPanel::drawPanelBackground() const {
        if (_style.getCornerRadius() > 0) {
            // Draw rounded rectangle
            DrawRectangleRounded(
                {_position.x, _position.y, _size.x, _size.y},
                _style.getCornerRadius() / _size.y,  // Normalize radius
                8,  // Segments for rounding
                _style.getBackgroundColor()
            );
        } else {
            // Draw regular rectangle
            DrawRectangle(
                _position.x,
                _position.y,
                _size.x,
                _size.y,
                _style.getBackgroundColor()
            );
        }
    }

    void UIPanel::drawPanelBorder() const {
        if (_style.getBorderThickness() <= 0) return;

        if (_style.getCornerRadius() > 0) {
            // Draw rounded rectangle border
            DrawRectangleRoundedLines(
                {_position.x, _position.y, _size.x, _size.y},
                _style.getCornerRadius() / _size.y,
                8,
                _style.getBorderColor()
            );
        } else {
            // Draw regular rectangle border
            DrawRectangleLinesEx(
                {_position.x, _position.y, _size.x, _size.y},
                _style.getBorderThickness(),
                _style.getBorderColor()
            );
        }
    }
}
