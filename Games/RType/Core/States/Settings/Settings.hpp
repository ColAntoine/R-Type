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

        bool blocks_update() const override { return false; }
        bool blocks_render() const override { return false; }

        std::string get_name() const override { return "Settings"; }
    private:
        void play_back_menu();
        void play_bind_menu();
        void play_audio_menu();
        void play_video_menu();
        void play_credit_menu();
};