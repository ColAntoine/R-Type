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
#include <vector>
#include <string>
#include <random>

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

    // ASCII background state
    std::vector<std::string> ascii_grid_;
    int ascii_cols_{0};
    int ascii_rows_{0};
    int ascii_font_size_{12};
    float ascii_timer_{0.0f};
    float ascii_interval_{0.04f}; // update ~25 FPS
    std::mt19937 ascii_rng_{std::random_device{}()};
    std::string ascii_charset_{" .,:;i!lI|/\\()1{}[]?-_+~<>^*abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};
    // menu appearance timing
    float menu_show_delay_{2.0f};
    float menu_elapsed_{0.0f};
    // new reveal (scanline + staggered) parameters
    float menu_reveal_duration_{0.9f}; // how long the reveal animation lasts
    float menu_reveal_progress_{0.0f}; // 0..1 progress during reveal
    float menu_flicker_timer_{0.0f};
};