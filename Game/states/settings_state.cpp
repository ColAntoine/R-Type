/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Settings Game State Implementation
*/

#include "settings_state.hpp"
#include "game_state_manager.hpp"
#include "../application.hpp"
#include <iostream>
#include <raylib.h>

SettingsState::SettingsState(Application* app) : app_(app) {
}

void SettingsState::enter() {
    setup_ui();
    initialized_ = true;
}

void SettingsState::exit() {
    cleanup_ui();
    initialized_ = false;
}

void SettingsState::pause() {
    ui_manager_.set_all_visible(false);
}

void SettingsState::resume() {
    ui_manager_.set_all_visible(true);
}

void SettingsState::update(float delta_time) {
    if (!initialized_) return;
    ui_manager_.update(delta_time);
}

void SettingsState::render() {
    if (!initialized_) return;

    // Draw semi-transparent overlay
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 100});

    // Render UI
    ui_manager_.render();
}

void SettingsState::handle_input() {
    if (!initialized_) return;

    // Handle escape key to go back
    if (IsKeyPressed(KEY_ESCAPE)) {
        on_back_clicked();
        return;
    }

    ui_manager_.handle_input();
}

void SettingsState::setup_ui() {
    // Get screen dimensions
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();

    float center_x = screen_width / 2.0f;
    float center_y = screen_height / 2.0f;

    // Settings panel
    auto settings_panel = std::make_shared<UIPanel>(center_x - 200, center_y - 150, 400, 300);
    settings_panel->set_background_color({50, 50, 70, 240});
    settings_panel->set_border_color({120, 120, 180, 255});
    ui_manager_.add_component("settings_panel", settings_panel);

    // Settings title
    auto title = std::make_shared<UIText>(center_x - 50, center_y - 120, "SETTINGS", 28);
    title->set_text_color({255, 255, 255, 255});
    ui_manager_.add_component("settings_title", title);

    // Player name section
    auto name_label = std::make_shared<UIText>(center_x - 180, center_y - 70, "Player Name:", 20);
    name_label->set_text_color({200, 200, 200, 255});
    ui_manager_.add_component("name_label", name_label);

    auto name_input = std::make_shared<UIInputField>(center_x - 180, center_y - 45, 250, 35, "Enter your name...");
    name_input->set_colors(
        {60, 60, 80, 255},    // Background
        {80, 80, 100, 255},   // Focused background
        {100, 100, 150, 255}, // Border
        {150, 150, 255, 255}  // Focused border
    );
    name_input->set_on_text_changed([this](const std::string& name) { on_player_name_changed(name); });
    ui_manager_.add_component("name_input", name_input);

    // Audio section placeholder
    auto audio_label = std::make_shared<UIText>(center_x - 180, center_y + 10, "Audio Volume: 100%", 20);
    audio_label->set_text_color({200, 200, 200, 255});
    ui_manager_.add_component("audio_label", audio_label);

    // Graphics section placeholder
    auto graphics_label = std::make_shared<UIText>(center_x - 180, center_y + 40, "Graphics: High", 20);
    graphics_label->set_text_color({200, 200, 200, 255});
    ui_manager_.add_component("graphics_label", graphics_label);

    // Back button
    auto back_button = std::make_shared<UIButton>(center_x - 75, center_y + 100, 150, 40, "BACK");
    back_button->set_colors(
        {100, 100, 100, 255}, // Normal
        {130, 130, 130, 255}, // Hovered
        {80, 80, 80, 255},    // Pressed
        {40, 40, 40, 255}     // Disabled
    );
    back_button->set_on_click([this]() { on_back_clicked(); });
    ui_manager_.add_component("back_button", back_button);
}

void SettingsState::cleanup_ui() {
    ui_manager_.clear_components();
}

void SettingsState::on_back_clicked() {
    if (state_manager_) {
        state_manager_->change_state("MainMenu");
    }
}

void SettingsState::on_player_name_changed(const std::string& name) {
    // Store the player name in the application
    if (app_ && !name.empty()) {
        app_->set_player_name(name);
    }
}