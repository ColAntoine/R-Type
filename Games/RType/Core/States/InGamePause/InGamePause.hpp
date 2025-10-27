#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Zipper.hpp"

#include "UI/Components/GlitchButton.hpp"

#include "Constants.hpp"

class InGamePauseState : public AGameState {

    struct FPSText {}; // Tag struct for quick access to FPS text

    public:
        InGamePauseState() = default;
        ~InGamePauseState() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;

        std::string get_name() const override { return "InGamePause"; }

        virtual bool blocks_update() const override { return false; }
        virtual bool blocks_render() const override { return false; }
};