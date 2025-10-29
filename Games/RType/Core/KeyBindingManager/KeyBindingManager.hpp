#pragma once

#include "ECS/Messaging/MessagingManager.hpp"
#include <string>
#include <map>

namespace EventTypes {
    const std::string KEY_PRESSED = "KEY_PRESSED";
    const std::string KEY_RELEASED = "KEY_RELEASED";
    const std::string MOUSE_PRESSED = "MOUSE_PRESSED";
    const std::string MOUSE_RELEASED = "MOUSE_RELEASED";
}

class KeyBindingManager {
    public:
        static KeyBindingManager& instance() {
            static KeyBindingManager instance;
            return instance;
        }

        void setKeyBinding(const std::string& action, int key);
        int getKeyBinding(const std::string& action) const;

        int stringToKeyCode(const std::string& keyStr) const;

        std::string getValueAssociatedToKeyCode(int) const;

        std::map<std::string, int> getKeyBindings() const;

        void checkAndEmitKeyEvents();
        void checkAndEmitMouseEvents();
    private:
        KeyBindingManager() = default;
        ~KeyBindingManager() = default;

        static const std::map<std::string, int> _stringToKeyMap;
        static const std::map<std::string, int> _mouseButtonMap;

        std::map<std::string, int> _keyBindings;

        std::map<int, bool> _previousKeyStates;
        std::map<int, bool> _previousMouseStates;
};
