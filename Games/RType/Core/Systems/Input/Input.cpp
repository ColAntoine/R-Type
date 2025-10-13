#include "Input.hpp"

// InputSystem implementation
InputSystem::InputSystem(EventManager* event_manager, entity player_entity)
    : event_manager_(event_manager), player_entity_(player_entity) {

    // Subscribe to input events
    event_manager_->subscribe<KeyPressEvent>([this](const KeyPressEvent& e) {
        handle_key_press(e.key);
    });

    event_manager_->subscribe<KeyReleaseEvent>([this](const KeyReleaseEvent& e) {
        handle_key_release(e.key);
    });
}

void InputSystem::update(registry& ecs_registry, __attribute_maybe_unused__ float delta_time) {
    // InputSystem only tracks key state and emits events; actual movement
    // is handled by the ControlSystem which reads `controllable` components.
    // No velocity modifications here.
}

void InputSystem::handle_key_press(int key) {
    switch (key) {
        case KEY_UP:
        case KEY_W:
            move_up_ = true;
            break;
        case KEY_DOWN:
        case KEY_S:
            move_down_ = true;
            break;
        case KEY_LEFT:
        case KEY_A:
            move_left_ = true;
            break;
        case KEY_RIGHT:
        case KEY_D:
            move_right_ = true;
            break;
    }
}

void InputSystem::handle_key_release(int key) {
    switch (key) {
        case KEY_UP:
        case KEY_W:
            move_up_ = false;
            break;
        case KEY_DOWN:
        case KEY_S:
            move_down_ = false;
            break;
        case KEY_LEFT:
        case KEY_A:
            move_left_ = false;
            break;
        case KEY_RIGHT:
        case KEY_D:
            move_right_ = false;
            break;
    }
}
