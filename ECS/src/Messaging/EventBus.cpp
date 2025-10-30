/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** EventBus Implementation
*/

#include "ECS/Messaging/EventBus.hpp"
#include <iostream>
#include <algorithm>

EventBus::CallbackId EventBus::subscribe(const std::string& type, EventCallback callback) {
    CallbackId id = next_id_++;
    subscribers_[type].push_back({id, type, callback});
    return id;
}

void EventBus::unsubscribe(CallbackId id) {
    for (auto& [type, subs] : subscribers_) {
        auto it = std::find_if(subs.begin(), subs.end(),
                               [id](const Subscription& s) { return s.id == id; });
        if (it != subs.end()) {
            subs.erase(it);
            return;
        }
    }
}

void EventBus::unsubscribe_deferred(CallbackId id) {
    pending_unsubscribes_.push_back(id);
}

void EventBus::emit(const Event& event) {
    auto it = subscribers_.find(event.type);
    if (it != subscribers_.end()) {
        for (const auto& sub : it->second) {
            try {
                sub.callback(event);
            } catch (const std::exception& e) {
                std::cerr << "Event callback error: " << e.what() << std::endl;
            }
        }
    }
}

void EventBus::emit_deferred(const Event& event) {
    deferred_events_.push(event);
}

void EventBus::process_deferred() {
    while (!deferred_events_.empty()) {
        Event event = deferred_events_.front();
        deferred_events_.pop();
        emit(event);
    }
    if (!pending_unsubscribes_.empty()) {
        for (auto id : pending_unsubscribes_) {
            unsubscribe(id);
        }
        pending_unsubscribes_.clear();
    }
}

void EventBus::clear_all() {
    subscribers_.clear();
    while (!deferred_events_.empty()) {
        deferred_events_.pop();
    }
    pending_unsubscribes_.clear();
}

size_t EventBus::get_subscriber_count(const std::string& type) const {
    auto it = subscribers_.find(type);
    return it != subscribers_.end() ? it->second.size() : 0;
}
