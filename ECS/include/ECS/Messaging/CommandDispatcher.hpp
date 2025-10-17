/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** CommandDispatcher - Request/Reply message passing subsystem
*/

#pragma once

#include "Events/Command.hpp"
#include <functional>
#include <unordered_map>
#include <vector>

using CommandHandler = std::function<void(Command&)>;

class CommandDispatcher {
    public:
        using HandlerId = size_t;

        HandlerId register_handler(const std::string& command_name, CommandHandler handler);
        void unregister_handler(HandlerId id);

        bool dispatch(Command& command);

        void clear_all();
        size_t get_handler_count(const std::string& command_name) const;

    private:
        struct Handler {
            HandlerId id;
            std::string command_name;
            CommandHandler handler;
        };

        std::unordered_map<std::string, std::vector<Handler>> handlers_;
        HandlerId next_id_;
};
