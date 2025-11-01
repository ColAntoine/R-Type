#include "KeyInput.hpp"
#include "ECS/Components.hpp"
#include "Entity/Components/Input/Input.hpp"
#include "ECS/Messaging/MessagingManager.hpp"
#include <raylib.h>
#include <iostream>

KeyInputSystem::KeyInputSystem() {
    auto& eventBus = MessagingManager::instance().get_event_bus();
    _callbackId = eventBus.subscribe(EventTypes::SET_KEY_BINDINGS,
        [this](const Event& event) { onSetKeyBindings(event); });
}

KeyInputSystem::~KeyInputSystem() {
    auto& eventBus = MessagingManager::instance().get_event_bus();
    eventBus.unsubscribe(_callbackId);
}

void KeyInputSystem::onSetKeyBindings(const Event& event) {
    _keyBinds = event.get<std::map<std::string, int>>("keyBindings");
}

void KeyInputSystem::update(registry& r, float dt) {
    auto* inputArr = r.get_if<Input>();
    if (!inputArr) return;

    for (auto& ctrl : *inputArr) {
        auto it_up = _keyBinds.find("move_up");
        ctrl.move_up = (it_up != _keyBinds.end()) && IsKeyDown(it_up->second);

        auto it_down = _keyBinds.find("move_down");
        ctrl.move_down = (it_down != _keyBinds.end()) && IsKeyDown(it_down->second);

        auto it_left = _keyBinds.find("move_left");
        ctrl.move_left = (it_left != _keyBinds.end()) && IsKeyDown(it_left->second);

        auto it_right = _keyBinds.find("move_right");
        ctrl.move_right = (it_right != _keyBinds.end()) && IsKeyDown(it_right->second);

        auto it_shoot = _keyBinds.find("shoot");
        ctrl.shoot = (it_shoot != _keyBinds.end()) && IsKeyDown(it_shoot->second);
    }
}

DLL_EXPORT ISystem* create_system() {
    try {
        return new KeyInputSystem();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}