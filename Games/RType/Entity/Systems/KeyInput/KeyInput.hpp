/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** KeyInput System
*/

#pragma once

#include "ECS/Systems/ISystem.hpp"
#include "ECS/Messaging/EventBus.hpp"
#include <map>
#include <string>

class KeyInputSystem : public ISystem {
public:
    KeyInputSystem();
    ~KeyInputSystem();
    void update(registry& r, float dt = 0.0f) override;
    const char* get_name() const override { return "KeyInputSystem"; }

private:
    void onSetKeyBindings(const Event& event);
    EventBus::CallbackId _callbackId;
    std::map<std::string, int> _keyBinds;
};