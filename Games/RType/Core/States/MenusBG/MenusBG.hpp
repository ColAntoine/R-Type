#pragma once

#include "Core/States/AGameState.hpp"

namespace RType {
    struct UIMainPanel : public IComponent {};
    struct UITitleText : public IComponent {};
    struct UIPlayButton : public IComponent {};
    struct UISettingsButton : public IComponent {};
    struct UIQuitButton : public IComponent {};
}

class MenusBackgroundState : public AGameState {
    public:
        MenusBackgroundState() = default;
        ~MenusBackgroundState() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void update(float delta_time) override;
        void render() override;

        std::string get_name() const override { return "MenusBackground"; }
    private:
        // MenusBackground specific members
        int _asciiFontSize = 12;
        int _asciiCols = 0;
        int _asciiRows = 0;
        float _asciiTimer = 0.0f;
        float _asciiInterval = 0.1f;
        std::string _asciiCharset = " .,:;i!lI|/\\()1{}[]?-_+~<>^*abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::vector<std::string> _asciiGrid;
};