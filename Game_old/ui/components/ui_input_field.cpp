/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI Input Field Component Implementation
*/

#include "ui_input_field.hpp"
#include <iostream>

void UIInputField::update(float delta_time) {
    if (!visible_) return;

    update_state();

    // Update cursor blink timer
    cursor_blink_timer_ += delta_time;
    if (cursor_blink_timer_ >= 1.0f) {
        show_cursor_ = !show_cursor_;
        cursor_blink_timer_ = 0.0f;
    }
}

void UIInputField::render() {
    if (!visible_) return;

    draw_input_background();
    draw_input_text();

    if (is_focused_ && show_cursor_) {
        draw_cursor();
    }
}

void UIInputField::handle_input() {
    if (!visible_ || !enabled_) return;

    Vector2 mouse_pos = GetMousePosition();
    bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    // Check if clicked inside or outside to set focus
    if (clicked) {
        bool was_focused = is_focused_;
        is_focused_ = is_point_inside(mouse_pos.x, mouse_pos.y);

        if (is_focused_ && !was_focused) {
            std::cout << "Input field focused" << std::endl;
        }
    }

    // Process text input only when focused
    if (is_focused_) {
        process_text_input();
    }
}

void UIInputField::process_text_input() {
    // Handle backspace
    if (IsKeyPressed(KEY_BACKSPACE) && !text_.empty()) {
        text_.pop_back();
        if (on_text_changed_) {
            on_text_changed_(text_);
        }
    }

    // Handle enter key
    if (IsKeyPressed(KEY_ENTER)) {
        if (on_enter_pressed_) {
            on_enter_pressed_(text_);
        }
        is_focused_ = false; // Unfocus after enter
    }

    // Handle escape key (unfocus)
    if (IsKeyPressed(KEY_ESCAPE)) {
        is_focused_ = false;
    }

    // Handle character input
    int key = GetCharPressed();
    while (key > 0) {
        // Check if character is printable and within length limit
        if (key >= 32 && key <= 126 && (int)text_.length() < max_length_) {
            text_ += static_cast<char>(key);
            if (on_text_changed_) {
                on_text_changed_(text_);
            }
        }
        key = GetCharPressed(); // Get next character
    }
}

void UIInputField::draw_input_background() {
    Color bg_color = is_focused_ ? focused_color_ : background_color_;
    Color border_color = is_focused_ ? focused_border_color_ : border_color_;

    // Draw background
    DrawRectangle(position_.x, position_.y, size_.x, size_.y, bg_color);

    // Draw border
    DrawRectangleLines(position_.x, position_.y, size_.x, size_.y, border_color);
}

void UIInputField::draw_input_text() {
    const float padding = 8.0f;
    std::string display_text = get_display_text();

    if (display_text.empty() && !placeholder_.empty() && !is_focused_) {
        // Draw placeholder text
        Vector2 text_pos = {position_.x + padding, position_.y + (size_.y - font_size_) / 2.0f};
        DrawTextEx(GetFontDefault(), placeholder_.c_str(), text_pos, font_size_, 1.0f, placeholder_color_);
    } else if (!display_text.empty()) {
        // Draw actual text
        Vector2 text_pos = {position_.x + padding, position_.y + (size_.y - font_size_) / 2.0f};
        DrawTextEx(GetFontDefault(), display_text.c_str(), text_pos, font_size_, 1.0f, text_color_);
    }
}

void UIInputField::draw_cursor() {
    const float padding = 8.0f;
    std::string display_text = get_display_text();
    Vector2 text_size = MeasureTextEx(GetFontDefault(), display_text.c_str(), font_size_, 1.0f);

    float cursor_x = position_.x + padding + text_size.x;
    float cursor_y = position_.y + (size_.y - font_size_) / 2.0f;

    DrawRectangle(cursor_x, cursor_y, 2, font_size_, text_color_);
}

std::string UIInputField::get_display_text() const {
    if (is_password_ && !text_.empty()) {
        return std::string(text_.length(), '*');
    }
    return text_;
}