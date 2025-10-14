/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI Button Component
*/

#pragma once

#include "../UIComponent.hpp"
#include <string>
#include <functional>

class UIButton : public UIComponent {
    private:
        std::string text_;
        std::function<void()> on_click_;

        // Visual properties
        Color normal_color_{70, 70, 70, 255};      // Dark gray
        Color hovered_color_{100, 100, 100, 255};  // Lighter gray
        Color pressed_color_{50, 50, 50, 255};     // Darker gray
        Color disabled_color_{40, 40, 40, 255};    // Very dark gray
        Color text_color_{255, 255, 255, 255};     // White text
        Color disabled_text_color_{150, 150, 150, 255}; // Gray text

        int font_size_{20};
        bool was_pressed_last_frame_{false};
    // Neon / glitch style
    Color neon_color_{0, 229, 255, 255}; // cyan neon
    Color neon_glow_color_{0, 229, 255, 100};
    float hover_jitter_amplitude_{2.0f};
    float hover_jitter_speed_{8.0f};
    bool enable_glitch_on_hover_{true};
    int hover_seed_{0};

    public:
        UIButton() = default;
        UIButton(float x, float y, float width, float height, const std::string& text)
            : UIComponent(x, y, width, height), text_(text) {}

        // IUIComponent implementation
        void update(float delta_time) override;
        void render() override;
        void handle_input() override;

        // Button-specific methods
        void set_text(const std::string& text) { text_ = text; }
        void set_on_click(std::function<void()> callback) { on_click_ = callback; }
        void set_colors(Color normal, Color hovered, Color pressed, Color disabled) {
            normal_color_ = normal;
            hovered_color_ = hovered;
            pressed_color_ = pressed;
            disabled_color_ = disabled;
        }
        void set_text_color(Color text_color, Color disabled_text_color) {
            text_color_ = text_color;
            disabled_text_color_ = disabled_text_color;
        }
        void set_neon(Color neon, Color glow) { neon_color_ = neon; neon_glow_color_ = glow; }
        void set_glitch_params(float amplitude, float speed, bool enabled=true) { hover_jitter_amplitude_ = amplitude; hover_jitter_speed_ = speed; enable_glitch_on_hover_ = enabled; }
        void set_font_size(int size) { font_size_ = size; }

        const std::string& get_text() const { return text_; }

    private:
        Color get_current_background_color() const;
        Color get_current_text_color() const;
        void draw_button_background() const;
        void draw_button_text() const;
};