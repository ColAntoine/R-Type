/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** CommandDispatcher Implementation
*/

#include "ECS/Messaging/CommandDispatcher.hpp"
#include <iostream>
#include <algorithm>

CommandDispatcher::HandlerId CommandDispatcher::register_handler(const std::string& command_name, CommandHandler handler) {
    HandlerId id = next_id_++;
    handlers_[command_name].push_back({id, command_name, handler});
    std::cout << "Registered command handler for: " << command_name << " (ID: " << id << ")" << std::endl;
    return id;
}

void CommandDispatcher::unregister_handler(HandlerId id) {
    for (auto& [name, handlers] : handlers_) {
        auto it = std::find_if(handlers.begin(), handlers.end(),
                               [id](const Handler& h) { return h.id == id; });
        if (it != handlers.end()) {
            std::cout << "Unregistered command handler: " << name << " (ID: " << id << ")" << std::endl;
            handlers.erase(it);
            return;
        }
    }
}

bool CommandDispatcher::dispatch(Command& command) {
    auto it = handlers_.find(command.command_name);
    if (it == handlers_.end() || it->second.empty()) {
        std::cerr << "No handler for command: " << command.command_name << std::endl;
        return false;
    }

    // For commands, typically only the first matching handler processes it
    // (unlike events where all subscribers get notified)
    try {
        it->second[0].handler(command);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Command handler error for '" << command.command_name << "': " << e.what() << std::endl;
        return false;
    }
}

void CommandDispatcher::clear_all() {
    handlers_.clear();
    std::cout << "Cleared all command handlers" << std::endl;
}

size_t CommandDispatcher::get_handler_count(const std::string& command_name) const {
    auto it = handlers_.find(command_name);
    return it != handlers_.end() ? it->second.size() : 0;
}
