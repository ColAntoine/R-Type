/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Control System
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"

class ControlSystem : public ISystem {
    public:
        void update(registry& r, float dt = 0.0f) override;
        const char* get_name() const override { return "ControlSystem"; }

        void setKeyBindings(const std::map<std::string, int>& bindings) {
            _keyBindings = bindings;
        }
    private:
        std::map<std::string, int> _keyBindings;
};

extern "C" {
    std::unique_ptr<ISystem> create_system();
}
