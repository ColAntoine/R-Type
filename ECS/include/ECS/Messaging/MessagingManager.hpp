/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** MessagingManager - Central messaging system (singleton)
** EVENTS: Something happened (async, broadcast to all subscribers)
** COMMANDS: Something must be done (sync, request/reply pattern)
*/

#pragma once

#include "EventBus.hpp"
#include "CommandDispatcher.hpp"
#include "MessageQueue.hpp"

class MessagingManager {
    public:
        static MessagingManager& instance();

    void init();
    void shutdown();
    void update();

    // EVENTS: Broadcast notifications (async, multiple subscribers)
    EventBus& get_event_bus() { return event_bus_; }

    // COMMANDS: Request/Reply pattern (sync, single handler)
    CommandDispatcher& get_command_dispatcher() { return command_dispatcher_; }

    // MESSAGE QUEUE: Thread-safe async message passing
    MessageQueue& get_message_queue() { return message_queue_; }        void clear_all();

    private:
        MessagingManager() = default;
        ~MessagingManager() = default;
    MessagingManager(const MessagingManager&) = delete;
    MessagingManager& operator=(const MessagingManager&) = delete;

    EventBus event_bus_;
    CommandDispatcher command_dispatcher_;
    MessageQueue message_queue_;
};