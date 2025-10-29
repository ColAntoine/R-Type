#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

#include "ECS/UI/UIBuilder.hpp"

#include "Constants.hpp"

class Browser : public AGameState {
    public:
        Browser() = default;
        ~Browser() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;

        std::string get_name() const override { return "Browser"; }

        virtual bool blocks_update() const override { return false; }
        virtual bool blocks_render() const override { return false; }

    private:
        void join_room_callback();
};