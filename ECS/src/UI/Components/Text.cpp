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
        if (_customRender) {
            _customRender(*this);
            return;
        }

        // Default rendering
        if (_style.hasShadow()) {
            drawTextShadow();
        }
        drawTextContent();
    }

    void UIText::handleInput() {
        // Text typically doesn't handle input
    }

    Vector2 UIText::getTextSize() const {
        return MeasureTextEx(
            GetFontDefault(),
            _text.c_str(),
            _style.getFontSize(),
            _style.getSpacing()
        );
    }

    void UIText::updateSize() {
        _size = getTextSize();
    }

    Vector2 UIText::calculateTextPosition() const {
        Vector2 text_size = getTextSize();
        Vector2 pos = _position;

        switch (_style.getAlignment()) {
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

    void UIText::drawTextShadow() const {
        if (!_style.hasShadow()) return;

        Vector2 pos = calculateTextPosition();
        Vector2 shadow_pos = {
            pos.x + _style.getShadowOffset().x,
            pos.y + _style.getShadowOffset().y
        };

        DrawTextEx(
            GetFontDefault(),
            _text.c_str(),
            shadow_pos,
            _style.getFontSize(),
            _style.getSpacing(),
            _style.getShadowColor()
        );
    }

    void UIText::drawTextContent() const {
        Vector2 pos = calculateTextPosition();

        DrawTextEx(
            GetFontDefault(),
            _text.c_str(),
            pos,
            _style.getFontSize(),
            _style.getSpacing(),
            _style.getTextColor()
        );
    }
}
