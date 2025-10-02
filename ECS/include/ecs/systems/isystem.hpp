/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** System Interface
*/

#pragma once

#include <memory>

class registry;

class ISystem {
public:
    virtual ~ISystem() = default;
    virtual void update(registry& r, float dt = 0.0f) = 0;
    virtual const char* get_name() const = 0;
};

// Factory function type for creating systems
typedef std::unique_ptr<ISystem> (*create_system_t)();
