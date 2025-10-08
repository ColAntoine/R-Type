#pragma once

#include "Core/EventManager.hpp"
#include "Core/Events.hpp"
#include "Core/Services/Input/Input.hpp"
#include "Core/Services/Network/Network.hpp"
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
