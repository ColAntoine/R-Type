#pragma once

#include "Core/States/GameState.hpp"

class InGameState : public IGameState {
    public:
        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void update(float delta_time) override;
        void render() override;
        void handle_input() override;

        std::string get_name() const override { return "InGame"; }
}