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
    public:
        // Getters
        Color getTextColor() const { return _text_color; }
        Color getShadowColor() const { return _shadow_color; }
        int getFontSize() const { return _font_size; }
        float getSpacing() const { return _spacing; }
        bool hasShadow() const { return _has_shadow; }
        Vector2 getShadowOffset() const { return _shadow_offset; }
        TextAlignment getAlignment() const { return _alignment; }

        // Setters
        void setTextColor(const Color& color) { _text_color = color; }
        void setShadowColor(const Color& color) { _shadow_color = color; }
        void setFontSize(int size) { _font_size = size; }
        void setSpacing(float spacing) { _spacing = spacing; }
        void setHasShadow(bool shadow) { _has_shadow = shadow; }
        void setShadowOffset(const Vector2& offset) { _shadow_offset = offset; }
        void setAlignment(TextAlignment alignment) { _alignment = alignment; }

    private:
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
            updateSize();
        }

        UIText(float x, float y, const std::string& text, int fontSize, Color textColor)
            : _text(text) {
            _position = {x, y};
            _style.setFontSize(fontSize);
            _style.setTextColor(textColor);
            // Size will be calculated based on text
            updateSize();
        }

        // IUIComponent implementation
        void update(float delta_time) override;
        void render() override;
        void handleInput() override;

        // Text specific methods
        void setText(const std::string& text) {
            _text = text;
            updateSize();
        }
        const std::string& getText() const { return _text; }

        void setStyle(const TextStyle& style) {
            _style = style;
            updateSize();
        }
        TextStyle& getStyle() { return _style; }
        const TextStyle& getStyle() const { return _style; }

        void setAlignment(TextAlignment alignment) { _style.setAlignment(alignment); }
        TextAlignment getAlignment() const { return _style.getAlignment(); }

        // Custom rendering callback
        void setCustomRender(std::function<void(const UIText&)> render_func) {
            _customRender = std::move(render_func);
        }

        // Get calculated text size
        Vector2 getTextSize() const;

    protected:
        // Virtual methods that can be overridden for custom styling
        virtual void drawTextShadow() const;
        virtual void drawTextContent() const;
        virtual Vector2 calculateTextPosition() const;

    public:
        TextStyle _style;

    private:
        void updateSize();

        std::string _text;
        std::function<void(const UIText&)> _customRender;
    };
}
