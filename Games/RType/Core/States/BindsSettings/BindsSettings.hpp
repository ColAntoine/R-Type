#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"
#include "ECS/Messaging/MessagingManager.hpp"

class BindsSettingsState : public AGameState {
    public:
        enum class CURRENT_BIND {
            UP = 0,
            DOWN,
            LEFT,
            RIGHT,
            SHOOT,
            NONE
        };

        BindsSettingsState() = default;
        ~BindsSettingsState() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;

        std::string get_name() const override { return "BindsSettings"; }

        bool blocks_update() const override { return false; }
        bool blocks_render() const override { return false; }

    private:
        std::string getTextFromButton(entity buttonEntity);
        void setTextToButton(entity buttonEntity, const std::string& text);

        void applyBinding(entity buttonEntity, const std::string &action, CURRENT_BIND toBind);

        std::string _currentText = "";
        EventBus::CallbackId _currentCallbackId;
        EventBus::CallbackId _mouseButtonCallbackId;

        CURRENT_BIND _bindToUpdate = CURRENT_BIND::NONE;

        entity _up;
        entity _down;
        entity _left;
        entity _right;
        entity _shoot;
};
