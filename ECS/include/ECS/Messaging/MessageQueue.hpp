/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** MessageQueue - Thread-safe message queue for inter-system communication
*/

#pragma once

#include <queue>
#include <mutex>
#include <string>
#include <any>
#include <unordered_map>
#include <stdexcept>

struct Message {
    std::string channel;
    std::unordered_map<std::string, std::any> payload;
    size_t sender_id;
    size_t target_id; // 0 = broadcast

    Message(const std::string& ch, size_t sender = 0, size_t target = 0)
        : channel(ch), sender_id(sender), target_id(target) {}

    template<typename T>
    void set(const std::string& key, const T& value) {
        payload[key] = value;
    }

    template<typename T>
    T get(const std::string& key) const {
        auto it = payload.find(key);
        if (it != payload.end()) {
            return std::any_cast<T>(it->second);
        }
        throw std::runtime_error("Message payload key not found: " + key);
    }

    template<typename T>
    T get_or(const std::string& key, const T& default_value) const {
        auto it = payload.find(key);
        if (it != payload.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (...) {
                return default_value;
            }
        }
        return default_value;
    }
};

class MessageQueue {
    public:
        void push(const Message& msg);
        bool pop(Message& out_msg);
        bool pop_for_channel(const std::string& channel, Message& out_msg);

        void clear();
        size_t size() const;
        bool empty() const;

    private:
        mutable std::mutex mutex_;
        std::queue<Message> queue_;
};
