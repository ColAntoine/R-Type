#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>
#include <queue>

// Base event class
struct Event {
    virtual ~Event() = default;
};

// Event handler type
template<typename T>
using EventHandler = std::function<void(const T&)>;

class EventManager {
    private:
        // Store handlers by event type
        std::unordered_map<std::type_index, std::vector<std::function<void(const Event&)>>> handlers_;

        // Event queue for deferred processing
        std::queue<std::unique_ptr<Event>> event_queue_;
        bool immediate_mode_ = true;

    public:
        EventManager() = default;
        ~EventManager() = default;

        // Subscribe to events
        template<typename T>
        void subscribe(EventHandler<T> handler) {
            static_assert(std::is_base_of_v<Event, T>, "T must inherit from Event");

            auto type_index = std::type_index(typeid(T));
            handlers_[type_index].push_back([handler](const Event& e) {
                handler(static_cast<const T&>(e));
            });
        }

        // Emit events (immediate or queued)
        template<typename T>
        void emit(const T& event) {
            static_assert(std::is_base_of_v<Event, T>, "T must inherit from Event");

            if (immediate_mode_) {
                dispatch_event(event);
            } else {
                event_queue_.push(std::make_unique<T>(event));
            }
        }

        // Process queued events - implementation in .cpp
        void process_events();

        // Control immediate vs queued processing - implementations in .cpp
        void set_immediate_mode(bool immediate);
        bool is_immediate_mode() const;

    private:
        template<typename T>
        void dispatch_event(const T& event) {
            auto type_index = std::type_index(typeid(T));
            auto it = handlers_.find(type_index);
            if (it != handlers_.end()) {
                for (auto& handler : it->second) {
                    handler(event);
                }
            }
        }
};