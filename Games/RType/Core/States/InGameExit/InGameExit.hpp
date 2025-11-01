#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

#include "ECS/Audio/AudioManager.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Zipper.hpp"

#include "UI/Components/GlitchButton.hpp"

#include "Constants.hpp"

class InGameExitState : public AGameState {

    public:
        InGameExitState() = default;
        ~InGameExitState() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;

        void update(float delta_time) override;

        std::string get_name() const override { return "InGameExit"; }

        virtual bool blocks_update() const override { return false; }
        virtual bool blocks_render() const override { return false; }
};
