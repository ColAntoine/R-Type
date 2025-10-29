#pragma once

#include <string>
#include <map>

class KeyBindingManager {
    public:
        static KeyBindingManager& instance() {
            static KeyBindingManager instance;
            return instance;
        }

        void setKeyBinding(const std::string& action, int key);
        int getKeyBinding(const std::string& action) const;

        int stringToKeyCode(const std::string& keyStr) const;

        std::map<std::string, int> getKeyBindings() const;
    private:
        KeyBindingManager() = default;
        ~KeyBindingManager() = default;

        static const std::map<std::string, int> _stringToKeyMap;

        std::map<std::string, int> _keyBindings;
};
