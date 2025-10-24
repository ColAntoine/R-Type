#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

class VideoSettingsState : public AGameState {
    public:
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
        // VideoSettingsState specific members
};
