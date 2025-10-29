#include "KeyBindingManager.hpp"

void KeyBindingManager::setKeyBinding(const std::string& action, int key)
{
    _keyBindings[action] = key;
}

int KeyBindingManager::getKeyBinding(const std::string& action) const
{
    auto it = _keyBindings.find(action);
    if (it != _keyBindings.end()) {
        return it->second;
    }
    return -1;
}

int KeyBindingManager::stringToKeyCode(const std::string& keyStr) const {
    auto it = _stringToKeyMap.find(keyStr);
    return (it != _stringToKeyMap.end()) ? it->second : KEY_NULL;
}
