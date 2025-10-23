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
        virtual void handleInput() = 0;

        // State management
        virtual void setVisible(bool visible) = 0;
        virtual bool isVisible() const = 0;

        virtual void setEnabled(bool enabled) = 0;
        virtual bool isEnabled() const = 0;

        // Position and size
        virtual void setPosition(float x, float y) = 0;
        virtual Vector2 getPosition() const = 0;

        virtual void setSize(float width, float height) = 0;
        virtual Vector2 getSize() const = 0;

        // Bounds checking
        virtual bool isPointInside(float x, float y) const = 0;

        // State
        virtual UIState getState() const = 0;
        virtual void setState(UIState state) = 0;
    };
}
