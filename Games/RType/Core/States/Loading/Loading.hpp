/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Loading State
*/

#pragma once

#include "Core/States/GameState.hpp"
#include "raylib.h"

class Loading : public IGameState {
    public:
        Loading();
        ~Loading() override;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void update(float delta_time) override;
        void render() override;
        void handle_input() override;

        std::string get_name() const override { return "Loading"; };

    private:
        Texture2D current_frame_texture_;
        int current_frame_;
        float animation_timer_;
        float frame_duration_;
        static constexpr int TOTAL_FRAMES = 122;
        static constexpr float FRAMES_PER_SECOND = 24.0f;
        
        void load_current_frame();
        void unload_current_frame();
};