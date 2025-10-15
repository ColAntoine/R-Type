#include "Input.hpp"

InputService::InputService(EventManager* event_manager) : event_manager_(event_manager) {
}

void InputService::initialize() {
    // Initialize key states
    for (int i = 0; i < 512; ++i) {
        previous_key_states_[i] = false;
        current_key_states_[i] = false;
    }
}

void InputService::shutdown() {
    // Nothing to clean up
}

void InputService::update([[maybe_unused]] float delta_time) {
    // Update key states and emit events
    update_keyboard();
    update_mouse();
}

void InputService::update_keyboard() {
    // Check common keys (expand as needed)
    int keys_to_check[] = {
        KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
        KEY_W, KEY_A, KEY_S, KEY_D,
        KEY_SPACE, KEY_ENTER, KEY_ESCAPE,
        KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR
    };

    for (int key : keys_to_check) {
        bool is_down = IsKeyDown(key);
        bool was_down = current_key_states_[key];

        current_key_states_[key] = is_down;

        // Emit events on state changes
        if (is_down && !was_down) {
            event_manager_->emit(KeyPressEvent(key));
        } else if (!is_down && was_down) {
            event_manager_->emit(KeyReleaseEvent(key));
        }
    }

    // Copy current to previous for next frame
    for (int key : keys_to_check) {
        previous_key_states_[key] = current_key_states_[key];
    }
}

void InputService::update_mouse() {
    Vector2 mouse_pos = GetMousePosition();

    // Mouse movement
    if (mouse_pos.x != previous_mouse_pos_.x || mouse_pos.y != previous_mouse_pos_.y) {
        event_manager_->emit(MouseMoveEvent(mouse_pos.x, mouse_pos.y));
    }

    // Mouse buttons
    int buttons[] = {MOUSE_BUTTON_LEFT, MOUSE_BUTTON_RIGHT, MOUSE_BUTTON_MIDDLE};
    for (int i = 0; i < 3; ++i) {
        int button = buttons[i];
        bool is_down = IsMouseButtonDown(button);
        bool was_down = previous_mouse_buttons_[button];

        if (is_down != was_down) {
            event_manager_->emit(MouseButtonEvent(button, is_down, mouse_pos.x, mouse_pos.y));
        }

        previous_mouse_buttons_[button] = is_down;
    }

    previous_mouse_pos_ = mouse_pos;
}