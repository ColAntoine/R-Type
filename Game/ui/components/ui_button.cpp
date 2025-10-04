/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI Button Component Implementation
*/

#include "ui_button.hpp"
#include <iostream>

void UIButton::update(float delta_time) {
    if (!visible_) return;
    
    update_state();
}

void UIButton::render() {
    if (!visible_) return;

    draw_button_background();
    draw_button_text();
}

void UIButton::handle_input() {
    if (!visible_ || !enabled_) return;

    Vector2 mouse_pos = GetMousePosition();
    bool is_hovered = is_point_inside(mouse_pos.x, mouse_pos.y);
    bool is_pressed = is_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    // Handle click event
    if (is_pressed && on_click_) {
        on_click_();
        std::cout << "Button clicked: " << text_ << std::endl;
    }
}

Color UIButton::get_current_background_color() const {
    switch (state_) {
        case UIState::Hovered:
            return hovered_color_;
        case UIState::Pressed:
            return pressed_color_;
        case UIState::Disabled:
            return disabled_color_;
        case UIState::Normal:
        default:
            return normal_color_;
    }
}

Color UIButton::get_current_text_color() const {
    return (state_ == UIState::Disabled) ? disabled_text_color_ : text_color_;
}

void UIButton::draw_button_background() const {
    Color bg_color = get_current_background_color();
    
    // Draw main button rectangle
    DrawRectangle(position_.x, position_.y, size_.x, size_.y, bg_color);
    
    // Draw border for better visibility
    Color border_color = {
        static_cast<unsigned char>(bg_color.r + 30),
        static_cast<unsigned char>(bg_color.g + 30),
        static_cast<unsigned char>(bg_color.b + 30),
        255
    };
    DrawRectangleLines(position_.x, position_.y, size_.x, size_.y, border_color);
    
    // Add subtle highlight for pressed state
    if (state_ == UIState::Pressed) {
        DrawRectangle(position_.x + 2, position_.y + 2, size_.x - 4, size_.y - 4, 
                     {255, 255, 255, 30}); // Semi-transparent white overlay
    }
}

void UIButton::draw_button_text() const {
    if (text_.empty()) return;

    Color text_color = get_current_text_color();
    
    // Calculate text position to center it in the button
    Vector2 text_size = MeasureTextEx(GetFontDefault(), text_.c_str(), font_size_, 1.0f);
    float text_x = position_.x + (size_.x - text_size.x) / 2.0f;
    float text_y = position_.y + (size_.y - text_size.y) / 2.0f;
    
    // Add slight offset when pressed for button press effect
    if (state_ == UIState::Pressed) {
        text_x += 1;
        text_y += 1;
    }
    
    DrawTextEx(GetFontDefault(), text_.c_str(), {text_x, text_y}, font_size_, 1.0f, text_color);
}