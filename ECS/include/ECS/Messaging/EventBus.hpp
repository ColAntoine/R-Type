/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** EventBus - Central event distribution system
*/

#pragma once

#include "Events/Event.hpp"
#include <functional>
#include <vector>
#include <queue>
#include <unordered_map>

using EventCallback = std::function<void(const Event&)>;

class EventBus {
    public:
        using CallbackId = size_t;

        CallbackId subscribe(EventType type, EventCallback callback);
        void unsubscribe(CallbackId id);

        void emit(const Event& event);
        void emit_deferred(const Event& event);

        void process_deferred();
        void clear_all();

        size_t get_subscriber_count(EventType type) const;
        size_t get_deferred_count() const { return deferred_events_.size(); }

    private:
        struct Subscription {
            CallbackId id;
            EventType type;
            EventCallback callback;
        };

        std::unordered_map<EventType, std::vector<Subscription>> subscribers_;
        std::queue<Event> deferred_events_;
        CallbackId next_id_;
};
