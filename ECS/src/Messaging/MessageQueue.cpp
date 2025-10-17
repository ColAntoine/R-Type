/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** MessageQueue Implementation
*/

#include "ECS/Messaging/MessageQueue.hpp"
#include <algorithm>

void MessageQueue::push(const Message& msg) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(msg);
}

bool MessageQueue::pop(Message& out_msg) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
        return false;
    }
    out_msg = queue_.front();
    queue_.pop();
    return true;
}

bool MessageQueue::pop_for_channel(const std::string& channel, Message& out_msg) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::queue<Message> temp;
    bool found = false;

    while (!queue_.empty()) {
        Message msg = queue_.front();
        queue_.pop();
        
        if (!found && msg.channel == channel) {
            out_msg = msg;
            found = true;
        } else {
            temp.push(msg);
        }
    }

    while (!temp.empty()) {
        queue_.push(temp.front());
        temp.pop();
    }

    return found;
}

void MessageQueue::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    while (!queue_.empty()) {
        queue_.pop();
    }
}

size_t MessageQueue::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

bool MessageQueue::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}
