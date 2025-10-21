#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"

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
        int _asciiFontSize = 18;
        int _asciiCols = 0;
        int _asciiRows = 0;
        float _asciiTimer = 0.0f;
        float _asciiInterval = 0.1f;
        std::string _asciiCharset = " .,:;i!lI|/\\()1{}[]?-_+~<>^*abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::vector<std::string> _asciiGrid;
        entity _asciiTextEntity;
};