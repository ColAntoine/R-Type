#pragma once

#include "IUIComponent.hpp"

// TODO: create AUIComponent in the src dir to make it more clear

namespace UI {
    class AUIComponent : public IUIComponent {
    public:
        AUIComponent() = default;
        virtual ~AUIComponent() = default;

        // IUIComponent implementation
        void setVisible(bool visible) override { _visible = visible; }
        bool isVisible() const override { return _visible; }

        void setEnabled(bool enabled) override { _enabled = enabled; }
        bool isEnabled() const override { return _enabled; }

        void setPosition(float x, float y) override {
            _position = {x, y};
        }

        Vector2 getPosition() const override {
            return _position;
        }

        void setSize(float width, float height) override {
            _size = {width, height};
        }

        Vector2 getSize() const override {
            return _size;
        }

        bool isPointInside(float x, float y) const override {
            return x >= _position.x && x <= _position.x + _size.x &&
                    y >= _position.y && y <= _position.y + _size.y;
        }

        UIState getState() const override { return _state; }
        void setState(UIState state) override { _state = state; }

    protected:
        void update_state() {
            if (!_enabled) {
                _state = UIState::Disabled;
                return;
            }

            Vector2 mouse_pos = GetMousePosition();
            bool is_hovered = isPointInside(mouse_pos.x, mouse_pos.y);
            bool is_pressed = is_hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

            if (is_pressed) {
                _state = UIState::Pressed;
            } else if (is_hovered) {
                _state = UIState::Hovered;
            } else {
                _state = UIState::Normal;
            }
        }

        Vector2 _position{0.0f, 0.0f};
        Vector2 _size{100.0f, 40.0f};
        UIState _state{UIState::Normal};
        bool _visible{true};
        bool _enabled{true};
    };
}
