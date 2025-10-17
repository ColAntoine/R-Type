/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Generic UI Text Component for ECS
*/

#pragma once

#include "../AUIComponent.hpp"
#include <string>
#include <functional>
#include <raylib.h>

namespace UI {

    enum class TextAlignment {
        Left,
        Center,
        Right
    };

    // Struct to define the base style of text
    struct TextStyle {
        Color _text_color{255, 255, 255, 255};
        Color _shadow_color{0, 0, 0, 150};

        int _font_size{20};
        float _spacing{1.0f};
        bool _has_shadow{false};
        Vector2 _shadow_offset{2.0f, 2.0f};
        TextAlignment _alignment{TextAlignment::Left};
    };

    class UIText : public AUIComponent {
    public:
        UIText() = default;

        UIText(float x, float y, const std::string& text)
            : _text(text) {
            _position = {x, y};
            // Size will be calculated based on text
            update_size();
        }

        // IUIComponent implementation
        void update(float delta_time) override;
        void render() override;
        void handle_input() override;

        // Text specific methods
        void set_text(const std::string& text) {
            _text = text;
            update_size();
        }
        const std::string& get_text() const { return _text; }

        void set_style(const TextStyle& style) {
            _style = style;
            update_size();
        }
        TextStyle& get_style() { return _style; }
        const TextStyle& get_style() const { return _style; }

        // Custom rendering callback
        void set_custom_render(std::function<void(const UIText&)> render_func) {
            _custom_render = std::move(render_func);
        }

        // Get calculated text size
        Vector2 get_text_size() const;

    protected:
        // Virtual methods that can be overridden for custom styling
        virtual void draw_text_shadow() const;
        virtual void draw_text_content() const;
        virtual Vector2 calculate_text_position() const;

    private:
        void update_size();

        std::string _text;
        TextStyle _style;
        std::function<void(const UIText&)> _custom_render;
    };
}
