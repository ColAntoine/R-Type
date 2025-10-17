#pragma once

#include <raylib.h>
#include <string>

namespace UI {
    enum class UIState {
        Normal,
        Hovered,
        Pressed,
        Disabled
    };

    class IUIComponent {
    public:
        virtual ~IUIComponent() = default;

        // Core UI methods
        virtual void update(float delta_time) = 0;
        virtual void render() = 0;
        virtual void handle_input() = 0;

        // State management
        virtual void set_visible(bool visible) = 0;
        virtual bool is_visible() const = 0;

        virtual void set_enabled(bool enabled) = 0;
        virtual bool is_enabled() const = 0;

        // Position and size
        virtual void set_position(float x, float y) = 0;
        virtual Vector2 get_position() const = 0;

        virtual void set_size(float width, float height) = 0;
        virtual Vector2 get_size() const = 0;

        // Bounds checking
        virtual bool is_point_inside(float x, float y) const = 0;

        // State
        virtual UIState get_state() const = 0;
        virtual void set_state(UIState state) = 0;
    };
}
