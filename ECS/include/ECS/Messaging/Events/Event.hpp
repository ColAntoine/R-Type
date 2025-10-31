/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** Event - Base event structure for messaging system
*/

#pragma once

#include <string>
#include <any>
#include <unordered_map>
#include <memory>
#include <stdexcept>

namespace EventTypes {
    const std::string ENTITY_CREATED = "ENTITY_CREATED";
    const std::string ENTITY_DESTROYED = "ENTITY_DESTROYED";
    const std::string COLLISION_ENTER = "COLLISION_ENTER";
    const std::string COLLISION_EXIT = "COLLISION_EXIT";
    const std::string SET_KEY_BINDINGS = "SET_KEY_BINDINGS";
}
struct Event {
    std::string type;
    size_t entity_id;
    std::unordered_map<std::string, std::any> data;

    Event(const std::string& t, size_t id = 0)
        : type(t), entity_id(id) {}

    template<typename T>
    void set(const std::string& key, const T& value) {
        data[key] = value;
    }

    template<typename T>
    T get(const std::string& key) const {
        auto it = data.find(key);
        if (it != data.end()) {
            return std::any_cast<T>(it->second);
        }
        throw std::runtime_error("Event data key not found: " + key);
    }

    template<typename T>
    T get_or(const std::string& key, const T& default_value) const {
        auto it = data.find(key);
        if (it != data.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (...) {
                return default_value;
            }
        }
        return default_value;
    }

    bool has(const std::string& key) const {
        return data.find(key) != data.end();
    }
};