#pragma once

#include "Core/States/AGameState.hpp"

namespace RType {
    struct UIMainPanel : public IComponent {};
    struct UITitleText : public IComponent {};
    struct UIPlayButton : public IComponent {};
    struct UISettingsButton : public IComponent {};
    struct UIQuitButton : public IComponent {};
}

class MainMenuState : public AGameState {
    public:
        MainMenuState() = default;
        ~MainMenuState() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;

        void update(float delta_time) override;

        bool blocks_update() const override { return false; }
        bool blocks_render() const override { return false; }

        std::string get_name() const override { return "MainMenu"; }
    private:
        void on_play_clicked();
        void on_settings_clicked();
        void on_quit_clicked();
        // MainMenu specific members
};