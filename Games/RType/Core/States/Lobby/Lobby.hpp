#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

#include "ECS/UI/UIBuilder.hpp"

#include "Constants.hpp"

class Lobby : public AGameState {

    struct ReadyButton: UI::UIButton {};

    public:
        Lobby() = default;
        ~Lobby() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;

        void update(float delta_time) override;

        std::string get_name() const override { return "Lobby"; }

        virtual bool blocks_update() const override { return false; }
        virtual bool blocks_render() const override { return false; }

    private:
};