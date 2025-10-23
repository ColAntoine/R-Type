#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"

class SettingsState : public AGameState {
    public:
        SettingsState() = default;
        ~SettingsState() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;

        void update(float delta_time) override;

        std::string get_name() const override { return "Settings"; }
    private:
        // Settings specific members
};