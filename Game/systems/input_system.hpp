#pragma once

#include "core/event_manager.hpp"
#include "core/events.hpp"
#include "services/input_service.hpp"
#include "services/network_service.hpp"
#include "ecs/registry.hpp"
#include "ecs/components.hpp"
#include "ecs/entity.hpp"
#include <unordered_map>

// ECS System that handles input events and updates player velocity
class InputSystem {
    private:
        EventManager* event_manager_;
        entity player_entity_;
        float movement_speed_ = 200.0f;

        // Track current input state
        bool move_up_ = false;
        bool move_down_ = false;
        bool move_left_ = false;
        bool move_right_ = false;

    public:
        InputSystem(EventManager* event_manager, entity player_entity);
        void update(registry& ecs_registry, float delta_time);

    private:
        void handle_key_press(int key);
        void handle_key_release(int key);
};
