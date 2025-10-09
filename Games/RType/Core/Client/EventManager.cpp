#include "EventManager.hpp"

void EventManager::process_events() {
    while (!event_queue_.empty()) {
        auto& event = event_queue_.front();
        dispatch_event(*event);
        event_queue_.pop();
    }
}

void EventManager::set_immediate_mode(bool immediate) {
    immediate_mode_ = immediate;
}

bool EventManager::is_immediate_mode() const {
    return immediate_mode_;
}