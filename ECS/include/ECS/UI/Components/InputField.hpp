/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Generic UI Input Field Component for ECS
*/

#pragma once

#include "../AUIComponent.hpp"
#include <string>
#include <functional>
#include <raylib.h>

namespace UI {

    // Struct to define the base style of input fields
    struct InputFieldStyle {
        public:
            // Getters
            Color getBackgroundColor() const { return _background_color; }
            Color getFocusedColor() const { return _focused_color; }
            Color getDisabledColor() const { return _disabled_color; }
            Color getBorderColor() const { return _border_color; }
            Color getFocusedBorderColor() const { return _focused_border_color; }
            Color getTextColor() const { return _text_color; }
            Color getPlaceholderColor() const { return _placeholder_color; }
            Color getDisabledTextColor() const { return _disabled_text_color; }
            Color getCursorColor() const { return _cursor_color; }
            int getFontSize() const { return _font_size; }
            float getBorderThickness() const { return _border_thickness; }
            float getPadding() const { return _padding; }
            float getCursorBlinkSpeed() const { return _cursor_blink_speed; }

            // Setters
            void setBackgroundColor(const Color& color) { _background_color = color; }
            void setFocusedColor(const Color& color) { _focused_color = color; }
            void setDisabledColor(const Color& color) { _disabled_color = color; }
            void setBorderColor(const Color& color) { _border_color = color; }
            void setFocusedBorderColor(const Color& color) { _focused_border_color = color; }
            void setTextColor(const Color& color) { _text_color = color; }
            void setPlaceholderColor(const Color& color) { _placeholder_color = color; }
            void setDisabledTextColor(const Color& color) { _disabled_text_color = color; }
            void setCursorColor(const Color& color) { _cursor_color = color; }
            void setFontSize(int font_size) { _font_size = font_size; }
            void setBorderThickness(float thickness) { _border_thickness = thickness; }
            void setPadding(float padding) { _padding = padding; }
            void setCursorBlinkSpeed(float speed) { _cursor_blink_speed = speed; }

        private:
            Color _background_color{50, 50, 50, 255};
            Color _focused_color{70, 70, 70, 255};
            Color _disabled_color{30, 30, 30, 255};
            Color _border_color{100, 100, 100, 255};
            Color _focused_border_color{150, 150, 255, 255};
            Color _text_color{255, 255, 255, 255};
            Color _placeholder_color{150, 150, 150, 255};
            Color _disabled_text_color{100, 100, 100, 255};
            Color _cursor_color{255, 255, 255, 255};

            int _font_size{20};
            float _border_thickness{1.0f};
            float _padding{10.0f};
            float _cursor_blink_speed{0.5f};
    };

    class UIInputField : public AUIComponent {
    public:
        UIInputField() = default;
        InputFieldStyle _style;

        UIInputField(float x, float y, float width, float height, const std::string& placeholder = "")
            : _placeholder(placeholder) {
            _position = {x, y};
            _size = {width, height};
        }

        std::string getText() { return _text; }

        // IUIComponent implementation
        void update(float delta_time) override;
        void render() override;
        void handleInput() override;

        // Input field specific methods
        void setText(const std::string& text) { _text = text; }
        const std::string& getText() const { return _text; }

        void setPlaceholder(const std::string& placeholder) { _placeholder = placeholder; }
        const std::string& getPlaceholder() const { return _placeholder; }

        void setPassword(bool is_password) { _is_password = is_password; }
        bool isPassword() const { return _is_password; }

        void setMaxLength(int max_length) { _max_length = max_length; }
        int getMaxLength() const { return _max_length; }

        void setFocused(bool focused) { _is_focused = focused; }
        bool isFocused() const { return _is_focused; }

        void setStyle(const InputFieldStyle& style) { _style = style; }
        InputFieldStyle& getStyle() { return _style; }
        const InputFieldStyle& getStyle() const { return _style; }

        // Callbacks
        void setOnTextChanged(std::function<void(const std::string&)> callback) {
            _on_text_changed = std::move(callback);
        }

        void setOnEnterPressed(std::function<void(const std::string&)> callback) {
            _on_enter_pressed = std::move(callback);
        }

        void setOnFocus(std::function<void()> callback) {
            _on_focus = std::move(callback);
        }

        void setOnFocusLost(std::function<void()> callback) {
            _on_focus_lost = std::move(callback);
        }

        // Custom rendering callback
        void setCustomRender(std::function<void(const UIInputField&)> render_func) {
            _customRender = std::move(render_func);
        }

    protected:
        virtual Color getCurrentBackgroundColor() const;
        virtual Color getCurrentBorderColor() const;
        virtual Color getCurrentTextColor() const;
        virtual void drawInputBackground() const;
        virtual void drawInputText() const;
        virtual void drawCursor() const;
        virtual std::string getDisplayText() const;

    private:
        void processTextInput();

        std::string _text;
        std::string _placeholder;

        bool _is_focused{false};
        bool _is_password{false};
        int _max_length{50};

        float _cursor_blink_timer{0.0f};
        bool _show_cursor{true};

        // Callbacks
        std::function<void(const std::string&)> _on_text_changed;
        std::function<void(const std::string&)> _on_enter_pressed;
        std::function<void()> _on_focus;
        std::function<void()> _on_focus_lost;
        std::function<void(const UIInputField&)> _customRender;

        // State tracking
        bool _was_focused_last_frame{false};
    };
}
