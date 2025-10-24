#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"
#include <string>

class VideoSettingsState : public AGameState {
    public:
        enum MoveDirection {
            Left,
            Right
        };
        struct Resolution {
            int width;
            int height;
        };

        VideoSettingsState() = default;
        ~VideoSettingsState() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;
        // void update(float delta_time) override;

        std::string get_name() const override { return "VideoSettings"; }

        bool blocks_update() const override { return false; }
        bool blocks_render() const override { return false; }
    private:
        std::string resToString(const Resolution& res) const;
        int _currentResIndex;
        int _nextResIndex;
        const std::vector<Resolution> _availableResolutions = {
            {1280, 720},
            {1920, 1080},
            {2560, 1440},
        };

        int _currentColorModeIndex;
        int _nextColorModeIndex;
        const std::vector<std::string> _availableColorModes = {
            "Default",
        };

        void apply_resolution_change(MoveDirection direction);
        // void apply_color_mode_change(MoveDirection direction);
};
