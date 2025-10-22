#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"

class InGameState : public AGameState {
    public:
        InGameState() = default;
        ~InGameState() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;

        void update(float delta_time) override;

        std::string get_name() const override { return "InGame"; }
    private:
        // InGame specific members
        entity _playerEntity;
};