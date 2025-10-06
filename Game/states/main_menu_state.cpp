/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Main Menu Game State Implementation
*/

#include "main_menu_state.hpp"
#include "game_state_manager.hpp"
#include <iostream>
#include <raylib.h>

void MainMenuState::enter() {
    setup_ui();
    initialized_ = true;
}

void MainMenuState::exit() {
    cleanup_ui();
    initialized_ = false;
}

void MainMenuState::pause() {
    // Hide UI when paused
    ui_manager_.set_all_visible(false);
}

void MainMenuState::resume() {
    // Show UI when resumed
    ui_manager_.set_all_visible(true);
}

void MainMenuState::update(float delta_time) {
    if (!initialized_) return;
    ui_manager_.update(delta_time);
}

void MainMenuState::render() {
    if (!initialized_) return;

    // Draw background
    ClearBackground({20, 20, 30, 255});

    // Draw some background decoration
    DrawText("R-TYPE", GetScreenWidth()/2 - 120, 50, 60, RAYWHITE);
    DrawText("A Classic Space Shooter", GetScreenWidth()/2 - 140, 120, 20, LIGHTGRAY);

    // Render UI
    ui_manager_.render();
}

void MainMenuState::handle_input() {
    if (!initialized_) return;
    ui_manager_.handle_input();
}

void MainMenuState::setup_ui() {
    // Get screen dimensions for responsive layout
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();

    // Calculate center positions
    float center_x = screen_width / 2.0f;
    float center_y = screen_height / 2.0f;

    // Main menu panel
    auto main_panel = std::make_shared<UIPanel>(center_x - 150, center_y - 100, 300, 250);
    main_panel->set_background_color({40, 40, 60, 200});
    main_panel->set_border_color({100, 100, 150, 255});
    ui_manager_.add_component("main_panel", main_panel);

    // Play button
    auto play_button = std::make_shared<UIButton>(center_x - 100, center_y - 50, 200, 50, "PLAY");
    play_button->set_colors(
        {70, 130, 180, 255},  // Normal - Steel blue
        {100, 160, 210, 255}, // Hovered
        {50, 110, 160, 255},  // Pressed
        {40, 40, 40, 255}     // Disabled
    );
    play_button->set_on_click([this]() { on_play_clicked(); });
    ui_manager_.add_component("play_button", play_button);

    // Settings button
    auto settings_button = std::make_shared<UIButton>(center_x - 100, center_y + 10, 200, 40, "SETTINGS");
    settings_button->set_colors(
        {100, 100, 100, 255}, // Normal - Gray
        {130, 130, 130, 255}, // Hovered
        {80, 80, 80, 255},    // Pressed
        {40, 40, 40, 255}     // Disabled
    );
    settings_button->set_on_click([this]() { on_settings_clicked(); });
    ui_manager_.add_component("settings_button", settings_button);

    // Quit button
    auto quit_button = std::make_shared<UIButton>(center_x - 100, center_y + 60, 200, 40, "QUIT");
    quit_button->set_colors(
        {180, 70, 70, 255},   // Normal - Red
        {210, 100, 100, 255}, // Hovered
        {160, 50, 50, 255},   // Pressed
        {40, 40, 40, 255}     // Disabled
    );
    quit_button->set_on_click([this]() { on_quit_clicked(); });
    ui_manager_.add_component("quit_button", quit_button);

}

void MainMenuState::cleanup_ui() {
    ui_manager_.clear_components();
}

void MainMenuState::on_play_clicked() {
    if (state_manager_) {
        state_manager_->change_state("InGame");
    }
}

void MainMenuState::on_settings_clicked() {
    if (state_manager_) {
        state_manager_->push_state("Settings");
    }
}

void MainMenuState::on_quit_clicked() {
    // Clear all states which should exit the game loop
    if (state_manager_) {
        state_manager_->clear_states();
    }
}