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

void InputSystem::update(registry& ecs_registry, float delta_time) {
    // Calculate velocity based on input state
    float vx = 0.0f;
    float vy = 0.0f;

    if (move_left_) vx -= movement_speed_;
    if (move_right_) vx += movement_speed_;
    if (move_up_) vy -= movement_speed_;
    if (move_down_) vy += movement_speed_;

    // Update player velocity component
    auto* vel_array = ecs_registry.get_if<velocity>();
    if (vel_array && vel_array->size() > static_cast<size_t>(player_entity_)) {
        (*vel_array)[static_cast<size_t>(player_entity_)] = velocity(vx, vy);
    }
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
