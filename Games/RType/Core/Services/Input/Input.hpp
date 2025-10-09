#pragma once

#include "Core/Services/ServiceManager.hpp"
#include "Core/Client/EventManager.hpp"
#include "Core/Client/Events.hpp"
#include <raylib.h>

class InputService : public IService {
    private:
        EventManager* event_manager_;

        // Track key states for edge detection
        bool previous_key_states_[512] = {false};
        bool current_key_states_[512] = {false};

        Vector2 previous_mouse_pos_ = {0, 0};
        bool previous_mouse_buttons_[8] = {false};

    public:
        InputService(EventManager* event_manager);

        void initialize() override;
        void shutdown() override;
        void update(float delta_time) override;

    private:
        void update_keyboard();
        void update_mouse();
};