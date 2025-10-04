/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Base UI Component Interface
*/

#pragma once

#include <raylib.h>
#include <functional>

// Forward declarations
class EventManager;

// Base interface for all UI components
class IUIComponent {
    public:
        virtual ~IUIComponent() = default;

        // Core UI methods
        virtual void update(float delta_time) = 0;
        virtual void render() = 0;
        virtual void handle_input() = 0;

        // Position and size management
        virtual void set_position(float x, float y) = 0;
        virtual void set_size(float width, float height) = 0;
        virtual Vector2 get_position() const = 0;
        virtual Vector2 get_size() const = 0;

        // Visibility and interaction
        virtual void set_visible(bool visible) = 0;
        virtual void set_enabled(bool enabled) = 0;
        virtual bool is_visible() const = 0;
        virtual bool is_enabled() const = 0;

        // Hit testing for mouse interaction
        virtual bool is_point_inside(float x, float y) const = 0;
};

// UI Component States
enum class UIState {
    Normal,
    Hovered,
    Pressed,
    Disabled
};

// Base implementation with common functionality
class UIComponent : public IUIComponent {
    protected:
        Vector2 position_{0.0f, 0.0f};
        Vector2 size_{100.0f, 30.0f};
        bool visible_{true};
        bool enabled_{true};
        UIState state_{UIState::Normal};

    public:
        UIComponent() = default;
        UIComponent(float x, float y, float width, float height)
            : position_{x, y}, size_{width, height} {}

        // Base implementations
        void set_position(float x, float y) override { position_ = {x, y}; }
        void set_size(float width, float height) override { size_ = {width, height}; }
        Vector2 get_position() const override { return position_; }
        Vector2 get_size() const override { return size_; }

        void set_visible(bool visible) override { visible_ = visible; }
        void set_enabled(bool enabled) override {
            enabled_ = enabled;
            if (!enabled_) state_ = UIState::Disabled;
        }
        bool is_visible() const override { return visible_; }
        bool is_enabled() const override { return enabled_; }

        bool is_point_inside(float x, float y) const override {
            return x >= position_.x && x <= position_.x + size_.x &&
                y >= position_.y && y <= position_.y + size_.y;
        }

        UIState get_state() const { return state_; }

    protected:
        void update_state() {
            if (!enabled_) {
                state_ = UIState::Disabled;
                return;
            }

            Vector2 mouse_pos = GetMousePosition();
            bool is_hovered = is_point_inside(mouse_pos.x, mouse_pos.y);
            bool is_pressed = is_hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

            if (is_pressed) { state_ = UIState::Pressed; }
            else if (is_hovered) { state_ = UIState::Hovered; }
            else { state_ = UIState::Normal; }
        }
};