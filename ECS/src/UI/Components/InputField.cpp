/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Generic UI Input Field Component Implementation
*/

#include "ECS/UI/Components/InputField.hpp"
#include "ECS/Renderer/RenderManager.hpp"
    #include <iostream>

namespace UI {
    void UIInputField::update(float delta_time) {
        if (!_visible) return;

        // Update cursor blink timer
        if (_is_focused) {
            _cursor_blink_timer += delta_time;
            if (_cursor_blink_timer >= _style.getCursorBlinkSpeed()) {
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
        if (_customRender) {
            _customRender(*this);
            return;
        }

        // Default rendering
        drawInputBackground();
        drawInputText();

        if (_is_focused && _show_cursor) {
            drawCursor();
        }
    }

    void UIInputField::handleInput() {
        if (!_visible || !_enabled) return;

        Vector2 mouse_pos = GetMousePosition();
        bool is_hovered = isPointInside(mouse_pos.x, mouse_pos.y);

        // Handle focus on click
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            _is_focused = is_hovered;
        }

        // Process text input if focused
        if (_is_focused) {
            processTextInput();
        }
    }

    void UIInputField::processTextInput() {
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

    Color UIInputField::getCurrentBackgroundColor() const {
        if (!_enabled) {
            return _style.getBackgroundColor();
        }
        return _is_focused ? _style.getFocusedColor() : _style.getBackgroundColor();
    }

    Color UIInputField::getCurrentBorderColor() const {
        if (!_enabled) {
            return _style.getBorderColor();
        }
        return _is_focused ? _style.getFocusedBorderColor() : _style.getBorderColor();
    }

    Color UIInputField::getCurrentTextColor() const {
        return _enabled ? _style.getTextColor() : _style.getDisabledTextColor();
    }

    void UIInputField::drawInputBackground() const {
        Color bg_color = getCurrentBackgroundColor();
        Color border_color = getCurrentBorderColor();

        // Draw background
        DrawRectangle(_position.x, _position.y, _size.x, _size.y, bg_color);

        // Draw border
        DrawRectangleLinesEx(
            {_position.x, _position.y, _size.x, _size.y},
            _style.getBorderThickness(),
            border_color
        );
    }


    void UIInputField::drawInputText() const {
        auto &renderManager = RenderManager::instance();
        std::string display_text = getDisplayText();
        Color text_color = getCurrentTextColor();

        // Show placeholder if empty
        std::cout << "display text: " << display_text << std::endl;
        if (display_text.empty() && !_placeholder.empty()) {
            display_text = _placeholder;
            text_color = _style.getPlaceholderColor();
        }

        if (display_text.empty()) return;

        // Calculate text position with padding
        float text_x = _position.x + _style.getPadding();
        float text_y = _position.y + (_size.y - _style.getFontSize()) / 2.0f;

        DrawTextEx(
            renderManager.get_font(),
            display_text.c_str(),
            {text_x, text_y},
            _style.getFontSize(),
            1.0f,
            text_color
        );
    }

    void UIInputField::drawCursor() const {
        auto &renderManager = RenderManager::instance();
        std::string display_text = getDisplayText();

        // Calculate cursor position
        float text_x = _position.x + _style.getPadding();
        float text_y = _position.y + (_size.y - _style.getFontSize()) / 2.0f;

        Vector2 text_size = MeasureTextEx(
            renderManager.get_font(),
            display_text.c_str(),
            _style.getFontSize(),
            1.0f
        );

        float cursor_x = text_x + text_size.x + 2.0f;
        float cursor_height = _style.getFontSize();

        // Draw cursor
        DrawRectangle(
            cursor_x,
            text_y,
            2.0f,
            cursor_height,
            _style.getCursorColor()
        );
    }

    std::string UIInputField::getDisplayText() const {
        std::cout << _text << std::endl;
        if (_is_password && !_text.empty()) {
            return std::string(_text.length(), '*');
        }
        return _text;
    }
}