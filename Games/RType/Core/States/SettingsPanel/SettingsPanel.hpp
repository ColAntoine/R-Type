#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

class SettingsPanelState : public AGameState {
    public:
        SettingsPanelState() = default;
        ~SettingsPanelState() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;

        bool blocks_update() const override { return false; }
        bool blocks_render() const override { return false; }

        std::string get_name() const override { return "settingspanel"; }
    private:
        // SettingsPanelState specific members
};
