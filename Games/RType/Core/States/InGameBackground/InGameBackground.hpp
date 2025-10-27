#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Zipper.hpp"

#include "Constants.hpp"
#include <vector>
#include <cmath>

class InGameBackground : public AGameState {

    struct FPSText {}; // Tag struct for quick access to FPS text

    public:
        InGameBackground() = default;
        ~InGameBackground() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;

        void update(float delta_time) override;

        std::string get_name() const override { return "InGameBackground"; }

        virtual bool blocks_update() const override { return false; }
        virtual bool blocks_render() const override { return false; }
};