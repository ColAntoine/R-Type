/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Main Menu Game State
*/

#pragma once

#include "Core/States/GameState.hpp"
#include "UI/UIManager.hpp"
#include "UI/Components/UIButton.hpp"
#include "UI/Components/UIText.hpp"
#include "UI/Components/UIPanel.hpp"
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