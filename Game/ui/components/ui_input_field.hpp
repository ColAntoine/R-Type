/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** UI Input Field Component
*/

#pragma once

#include "../ui_component.hpp"
#include <string>
#include <functional>

class UIInputField : public UIComponent {
    private:
        std::string text_;
        std::string placeholder_;
        bool is_focused_{false};
        bool is_password_{false};

        // Visual properties
        Color background_color_{50, 50, 50, 255};
        Color focused_color_{70, 70, 70, 255};
        Color border_color_{100, 100, 100, 255};
        Color focused_border_color_{150, 150, 255, 255};
        Color text_color_{255, 255, 255, 255};
        Color placeholder_color_{150, 150, 150, 255};

        int font_size_{20};
        float cursor_blink_timer_{0.0f};
        bool show_cursor_{true};
        int max_length_{50};

        // Callbacks
        std::function<void(const std::string&)> on_text_changed_;
        std::function<void(const std::string&)> on_enter_pressed_;

    public:
        UIInputField() = default;
        UIInputField(float x, float y, float width, float height, const std::string& placeholder = "")
            : UIComponent(x, y, width, height), placeholder_(placeholder) {}

        // IUIComponent implementation
        void update(float delta_time) override;
        void render() override;
        void handle_input() override;

        // Input field specific methods
        void set_text(const std::string& text) { text_ = text; }
        void set_placeholder(const std::string& placeholder) { placeholder_ = placeholder; }
        void set_password(bool is_password) { is_password_ = is_password; }
        void set_max_length(int max_length) { max_length_ = max_length; }
        void set_focused(bool focused) { is_focused_ = focused; }

        void set_colors(Color bg, Color focused_bg, Color border, Color focused_border) {
            background_color_ = bg;
            focused_color_ = focused_bg;
            border_color_ = border;
            focused_border_color_ = focused_border;
        }

        void set_text_color(Color text_color, Color placeholder_color) {
            text_color_ = text_color;
            placeholder_color_ = placeholder_color;
        }

        void set_on_text_changed(std::function<void(const std::string&)> callback) {
            on_text_changed_ = callback;
        }

        void set_on_enter_pressed(std::function<void(const std::string&)> callback) {
            on_enter_pressed_ = callback;
        }

        const std::string& get_text() const { return text_; }
        bool is_focused() const { return is_focused_; }

    private:
        void process_text_input();
        void draw_input_background();
        void draw_input_text();
        void draw_cursor();
        std::string get_display_text() const;
};