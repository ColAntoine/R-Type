/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** In-Game State - Wraps existing game logic
*/

#pragma once

#include "Core/States/GameState.hpp"
#include "UI/UIManager.hpp"
#include "UI/Components/UIText.hpp"
#include <memory>

// Forward declarations
class Application;

class InGameState : public IGameState {
    private:
        Application* app_;
        UIManager ui_manager_;
        bool initialized_{false};
        bool paused_{false};

    public:
        InGameState(Application* app);
        ~InGameState() override = default;

        // IGameState implementation
        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void update(float delta_time) override;
        void render() override;
        void handle_input() override;

        std::string get_name() const override { return "InGame"; }
        bool blocks_update() const override { return true; }
        bool blocks_render() const override { return true; }

    private:
        void setup_hud();
        void cleanup_hud();
        void update_hud();
};