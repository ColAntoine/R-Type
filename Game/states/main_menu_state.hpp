/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Main Menu Game State
*/

#pragma once

#include "game_state.hpp"
#include "../ui/ui_manager.hpp"
#include "../ui/components/ui_button.hpp"
#include "../ui/components/ui_text.hpp"
#include "../ui/components/ui_panel.hpp"
#include <memory>

class MainMenuState : public IGameState {
private:
    UIManager ui_manager_;
    bool initialized_{false};

public:
    MainMenuState() = default;
    ~MainMenuState() override = default;

    // IGameState implementation
    void enter() override;
    void exit() override;
    void pause() override;
    void resume() override;

    void update(float delta_time) override;
    void render() override;
    void handle_input() override;

    std::string get_name() const override { return "MainMenu"; }

private:
    void setup_ui();
    void cleanup_ui();

    // Button callbacks
    void on_play_clicked();
    void on_settings_clicked();
    void on_quit_clicked();
};