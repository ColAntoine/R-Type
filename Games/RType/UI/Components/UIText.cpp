/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI Text Component Implementation
*/

#include "UIText.hpp"

void UIText::update(float delta_time) {
    // Text components typically don't need per-frame updates
    // but we keep this for consistency with the UI interface
}

void UIText::render() {
    if (!visible_ || text_.empty()) return;

    Vector2 text_pos = get_text_position();
    DrawTextEx(GetFontDefault(), text_.c_str(), text_pos, font_size_, 1.0f, text_color_);
}

void UIText::handle_input() {
    // Text components don't handle input by default
    // Override this in derived classes if needed (e.g., clickable text/links)
}

void UIText::update_size() {
    if (text_.empty()) {
        size_ = {0, 0};
        return;
    }

    Vector2 text_size = MeasureTextEx(GetFontDefault(), text_.c_str(), font_size_, 1.0f);
    size_ = text_size;
}

Vector2 UIText::get_text_position() const {
    Vector2 text_pos = position_;

    if (alignment_ == TextAlignment::Center) {
        Vector2 text_size = MeasureTextEx(GetFontDefault(), text_.c_str(), font_size_, 1.0f);
        text_pos.x = position_.x + (size_.x - text_size.x) / 2.0f;
    } else if (alignment_ == TextAlignment::Right) {
        Vector2 text_size = MeasureTextEx(GetFontDefault(), text_.c_str(), font_size_, 1.0f);
        text_pos.x = position_.x + size_.x - text_size.x;
    }
    // Left alignment uses position_.x directly (default)

    return text_pos;
}