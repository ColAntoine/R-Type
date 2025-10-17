/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** Command - Request/Reply pattern for synchronous operations
*/

#pragma once

#include <string>
#include <any>
#include <unordered_map>
#include <memory>
#include <stdexcept>

enum class CommandType {
    REQUEST_ENTITY_INFO,
    REQUEST_COMPONENT_DATA,
    REQUEST_SYSTEM_STATE,
    CUSTOM
};

struct Command {
    CommandType type;
    std::string command_name;
    size_t sender_id;
    size_t target_id;
    std::unordered_map<std::string, std::any> params;
    std::unordered_map<std::string, std::any> reply;
    bool has_reply;

    Command(CommandType t, const std::string& name, size_t sender = 0, size_t target = 0)
        : type(t), command_name(name), sender_id(sender), target_id(target), has_reply(false) {}

    template<typename T>
    void set_param(const std::string& key, const T& value) {
        params[key] = value;
    }

    template<typename T>
    T get_param(const std::string& key) const {
        auto it = params.find(key);
        if (it != params.end()) {
            return std::any_cast<T>(it->second);
        }
        throw std::runtime_error("Command param key not found: " + key);
    }

    template<typename T>
    void set_reply(const std::string& key, const T& value) {
        reply[key] = value;
        has_reply = true;
    }

    template<typename T>
    T get_reply(const std::string& key) const {
        auto it = reply.find(key);
        if (it != reply.end()) {
            return std::any_cast<T>(it->second);
        }
        throw std::runtime_error("Command reply key not found: " + key);
    }

    template<typename T>
    T get_reply_or(const std::string& key, const T& default_value) const {
        auto it = reply.find(key);
        if (it != reply.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (...) {
                return default_value;
            }
        }
        return default_value;
    }
};
