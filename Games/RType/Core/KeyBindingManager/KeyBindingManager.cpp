#include "KeyBindingManager.hpp"
#include <raylib.h>

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

int KeyBindingManager::stringToKeyCode(const std::string& keyStr) const
{
    auto it = _stringToKeyMap.find(keyStr);
    return (it != _stringToKeyMap.end()) ? it->second : KEY_NULL;
}

std::string KeyBindingManager::getValueAssociatedToKeyCode(int keyCode) const
{
    for (const auto &[key, val] : _stringToKeyMap) {
        if (val == keyCode)
            return key;
    }
    return "NULL";
}

std::map<std::string, int> KeyBindingManager::getKeyBindings() const
{
    return _keyBindings;
}

void KeyBindingManager::checkAndEmitKeyEvents()
{
    auto& eventBus = MessagingManager::instance().get_event_bus();
    for (const auto& [key, value] : _stringToKeyMap) {
        bool isDown = IsKeyDown(value);
        bool wasDown = _previousKeyStates[value];

        if (isDown && !wasDown) {
            // Special handling for ESCAPE key
            if (key == "ESCAPE") {
                Event escapePressed(EventTypes::ESCAPE_KEY_PRESSED);
                eventBus.emit(escapePressed);
            }

            Event keyPressed(EventTypes::KEY_PRESSED);
            keyPressed.set("key", key);
            eventBus.emit(keyPressed);
        } else if (!isDown && wasDown) {
            Event keyReleased(EventTypes::KEY_RELEASED);
            keyReleased.set("key", key);
            eventBus.emit(keyReleased);
        }
        _previousKeyStates[value] = isDown;
    }
}

void KeyBindingManager::checkAndEmitMouseEvents()
{
    auto& eventBus = MessagingManager::instance().get_event_bus();
    for (const auto& [name, button] : _mouseButtonMap) {
        bool isDown = IsMouseButtonDown(button);
        bool wasDown = _previousMouseStates[button];

        if (isDown && !wasDown) {
            Event mousePressed(EventTypes::MOUSE_PRESSED);
            mousePressed.set("button", name);
            eventBus.emit(mousePressed);
        } else if (!isDown && wasDown) {
            Event mouseReleased(EventTypes::MOUSE_RELEASED);
            mouseReleased.set("button", name);
            eventBus.emit(mouseReleased);
        }
        _previousMouseStates[button] = isDown;
    }
}

const std::map<std::string, int> KeyBindingManager::_stringToKeyMap = {
    {"A", KEY_A}, {"B", KEY_B}, {"C", KEY_C}, {"D", KEY_D}, {"E", KEY_E},
    {"F", KEY_F}, {"G", KEY_G}, {"H", KEY_H}, {"I", KEY_I}, {"J", KEY_J},
    {"K", KEY_K}, {"L", KEY_L}, {"M", KEY_M}, {"N", KEY_N}, {"O", KEY_O},
    {"P", KEY_P}, {"Q", KEY_Q}, {"R", KEY_R}, {"S", KEY_S}, {"T", KEY_T},
    {"U", KEY_U}, {"V", KEY_V}, {"W", KEY_W}, {"X", KEY_X}, {"Y", KEY_Y},
    {"Z", KEY_Z},

    {"ZERO", KEY_ZERO}, {"ONE", KEY_ONE}, {"TWO", KEY_TWO}, {"THREE", KEY_THREE},
    {"FOUR", KEY_FOUR}, {"FIVE", KEY_FIVE}, {"SIX", KEY_SIX}, {"SEVEN", KEY_SEVEN},
    {"EIGHT", KEY_EIGHT}, {"NINE", KEY_NINE},

    {"SPACE", KEY_SPACE}, {"ENTER", KEY_ENTER}, {"TAB", KEY_TAB},
    {"BACKSPACE", KEY_BACKSPACE}, {"ESCAPE", KEY_ESCAPE}, {"DELETE", KEY_DELETE},
    {"INSERT", KEY_INSERT}, {"HOME", KEY_HOME}, {"END", KEY_END}, {"PAGE_UP", KEY_PAGE_UP},
    {"PAGE_DOWN", KEY_PAGE_DOWN},

    {"UP", KEY_UP}, {"DOWN", KEY_DOWN}, {"LEFT", KEY_LEFT}, {"RIGHT", KEY_RIGHT},

    {"F1", KEY_F1}, {"F2", KEY_F2}, {"F3", KEY_F3}, {"F4", KEY_F4},
    {"F5", KEY_F5}, {"F6", KEY_F6}, {"F7", KEY_F7}, {"F8", KEY_F8},
    {"F9", KEY_F9}, {"F10", KEY_F10}, {"F11", KEY_F11}, {"F12", KEY_F12},

    {"APOSTROPHE", KEY_APOSTROPHE}, {"COMMA", KEY_COMMA}, {"MINUS", KEY_MINUS},
    {"PERIOD", KEY_PERIOD}, {"SLASH", KEY_SLASH}, {"SEMICOLON", KEY_SEMICOLON},
    {"EQUAL", KEY_EQUAL}, {"LEFT_BRACKET", KEY_LEFT_BRACKET}, {"RIGHT_BRACKET", KEY_RIGHT_BRACKET},
    {"BACKSLASH", KEY_BACKSLASH}, {"GRAVE", KEY_GRAVE},

    {"KP_0", KEY_KP_0}, {"KP_1", KEY_KP_1}, {"KP_2", KEY_KP_2}, {"KP_3", KEY_KP_3},
    {"KP_4", KEY_KP_4}, {"KP_5", KEY_KP_5}, {"KP_6", KEY_KP_6}, {"KP_7", KEY_KP_7},
    {"KP_8", KEY_KP_8}, {"KP_9", KEY_KP_9}, {"KP_DECIMAL", KEY_KP_DECIMAL},
    {"KP_DIVIDE", KEY_KP_DIVIDE}, {"KP_MULTIPLY", KEY_KP_MULTIPLY}, {"KP_SUBTRACT", KEY_KP_SUBTRACT},
    {"KP_ADD", KEY_KP_ADD}, {"KP_ENTER", KEY_KP_ENTER}, {"KP_EQUAL", KEY_KP_EQUAL}
};

const std::map<std::string, int> KeyBindingManager::_mouseButtonMap = {
    {"LEFT", MOUSE_LEFT_BUTTON},
    {"RIGHT", MOUSE_RIGHT_BUTTON},
    {"MIDDLE", MOUSE_MIDDLE_BUTTON},
};
