#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"
#include "UI/ColorPalette.hpp"
#include <string>
#include <tuple>

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

        std::string get_name() const override { return "VideoSettings"; }

        bool blocks_update() const override { return false; }
        bool blocks_render() const override { return false; }
    private:
        std::string resToString(const Resolution& res) const;
        void applyResChange(MoveDirection);
        void applyColorChange(MoveDirection);
        void filter_available_resolutions(int monitorWidth, int monitorHeight);
        void set_current_resolution_index(int currentWidth, int currentHeight);

        size_t _resolutionIndex;
        std::vector<Resolution> _availableResolution = {
            {1280, 720},
            {1920, 1080}
        };

        size_t _colorModeIndex;
        const std::vector<std::tuple<std::string, ColorPalette>> _availableColorModes = {
            {"Default", DefaultPalette},
            {"BlackText", BlackText}
        };
};
