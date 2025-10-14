/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI Button Component Implementation
*/

#include "UIButton.hpp"
#include <iostream>
#include <cmath>

void UIButton::update(__attribute_maybe_unused__ float delta_time) {
    if (!visible_) return;

    update_state();
}

void UIButton::render() {
    if (!visible_) return;

    draw_button_background();
    draw_button_text();

    // When hovered, add subtle neon glow
    if (state_ == UIState::Hovered) {
        // glow rectangle
        DrawRectangleLinesEx({position_.x - 2, position_.y - 2, size_.x + 4, size_.y + 4}, 2.0f, neon_color_);
        DrawRectangle(position_.x - 4, position_.y - 4, size_.x + 8, 4, neon_glow_color_);
    }
}

void UIButton::handle_input() {
    if (!visible_ || !enabled_) return;

    Vector2 mouse_pos = GetMousePosition();
    bool is_hovered = is_point_inside(mouse_pos.x, mouse_pos.y);
    bool is_pressed = is_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    // Handle click event
    if (is_pressed && on_click_) {
        on_click_();
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

    // add faint scanline texture for cyberpunk feel
    for (int i = 0; i < (int)size_.y; i += 6) {
        DrawRectangle(position_.x, position_.y + i, size_.x, 1, {0, 0, 0, 10});
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
    
    // On hover, apply jitter and RGB-split glitch
    if (state_ == UIState::Hovered && enable_glitch_on_hover_) {
        float t = GetTime() * hover_jitter_speed_ + hover_seed_;
        float jitter_x = std::sin(t) * hover_jitter_amplitude_;
        float jitter_y = std::cos(t * 1.3f) * (hover_jitter_amplitude_ / 2.0f);

        // red layer
        Color rcol = {255, 50, 80, text_color.a};
        DrawTextEx(GetFontDefault(), text_.c_str(), {text_x + jitter_x + 1, text_y + jitter_y}, font_size_, 1.0f, rcol);
        // green layer
        Color gcol = {0, 255, 156, (unsigned char)(text_color.a * 0.6f)};
        DrawTextEx(GetFontDefault(), text_.c_str(), {text_x - jitter_x - 1, text_y - jitter_y}, font_size_, 1.0f, gcol);
        // main layer
        DrawTextEx(GetFontDefault(), text_.c_str(), {text_x, text_y}, font_size_, 1.0f, text_color);

        // tiny random scanline flicker overlay
        if (((int)(GetTime() * 10) + hover_seed_) % 5 == 0) {
            DrawRectangle(text_x - 4, text_y + text_size.y + 2, text_size.x + 8, 2, {255, 255, 255, 10});
        }
    } else {
        DrawTextEx(GetFontDefault(), text_.c_str(), {text_x, text_y}, font_size_, 1.0f, text_color);
    }
}