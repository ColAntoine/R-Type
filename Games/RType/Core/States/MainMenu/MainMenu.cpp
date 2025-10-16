/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Main Menu Game State Implementation
*/

#include "MainMenu.hpp"
#include "Core/States/GameStateManager.hpp"
#include <iostream>
#include <raylib.h>
#include <cmath>
#include <iomanip>

void MainMenuState::enter() {
    // prepare ascii grid based on screen size and font size
    ascii_font_size_ = 12;
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int font_w = std::max(1, ascii_font_size_ / 2);
    int font_h = ascii_font_size_;
    ascii_cols_ = std::max(10, (sw + font_w - 1) / font_w);
    ascii_rows_ = std::max(8, (sh + font_h - 1) / font_h);
    ascii_grid_.assign(ascii_rows_, std::string(ascii_cols_, ' '));

    setup_ui();

    // start with UI hidden; we'll reveal it after a short delay with a staged animation
    ui_manager_.set_all_visible(false);
    menu_elapsed_ = 0.0f;
    menu_reveal_progress_ = 0.0f;
    menu_flicker_timer_ = 0.0f;
    // randomize a seed for hover jitter seeds used by buttons
    for (size_t i = 0; i < ui_manager_.get_component_count(); ++i) {
        // best-effort: set hover seed if component is a UIButton
        // use get_component by name when needed; we keep this minimal here
    }

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

    // update ascii background timer
    ascii_timer_ += delta_time;
    if (ascii_timer_ >= ascii_interval_) {
        ascii_timer_ = 0.0f;
        // randomize a few cells
        std::uniform_int_distribution<int> row_dist(0, ascii_rows_ - 1);
        std::uniform_int_distribution<int> col_dist(0, ascii_cols_ - 1);
        std::uniform_int_distribution<int> char_dist(0, (int)ascii_charset_.size() - 1);
        int changes = std::max(1, (ascii_cols_ * ascii_rows_) / 40);
        for (int i = 0; i < changes; ++i) {
            int r = row_dist(ascii_rng_);
            int c = col_dist(ascii_rng_);
            ascii_grid_[r][c] = ascii_charset_[char_dist(ascii_rng_)];
        }
    }

    // Menu reveal logic: after menu_show_delay_, start a short flicker then reveal
    menu_elapsed_ += delta_time;
    // reveal start/time window
    if (menu_elapsed_ >= menu_show_delay_) {
        // advance reveal progress
        float t = menu_elapsed_ - menu_show_delay_;
        menu_reveal_progress_ = std::min(1.0f, t / menu_reveal_duration_);

        // staged reveal thresholds for components
        // 0.0 -> panel, 0.33 -> play, 0.66 -> settings, 1.0 -> quit
        auto panel = ui_manager_.get_component("main_panel");
        auto play = ui_manager_.get_component<UIButton>("play_button");
        auto settings = ui_manager_.get_component<UIButton>("settings_button");
        auto quit = ui_manager_.get_component<UIButton>("quit_button");

        if (panel) panel->set_visible(menu_reveal_progress_ >= 0.0f);
        if (play) play->set_visible(menu_reveal_progress_ >= 0.33f);
        if (settings) settings->set_visible(menu_reveal_progress_ >= 0.66f);
        if (quit) quit->set_visible(menu_reveal_progress_ >= 1.0f - 1e-6f);
    }
}

void MainMenuState::render() {
    if (!initialized_) return;

    // Dark background
    ClearBackground({8, 10, 12, 255});

    // ASCII glitch background (draw behind)
    // draw from top-left so background fully covers the screen
    int start_x = 0;
    int start_y = 0;
    Color ascii_color = {0, 229, 255, 90}; // cyan-ish low alpha
    for (int r = 0; r < ascii_rows_; ++r) {
        for (int c = 0; c < ascii_cols_; ++c) {
            char ch = ascii_grid_[r][c];
            if (ch == ' ') continue;
            int x = start_x + c * (ascii_font_size_ / 2);
            int y = start_y + r * ascii_font_size_;
            DrawText(std::string(1, ch).c_str(), x, y, ascii_font_size_, ascii_color);
        }
    }

    // Dark translucent overlay to make UI readable
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 120});

    // Title with small glitch offset layers
    std::string title = "    R-TYPE";
    int tx = GetScreenWidth() / 2 - 200;
    int ty = 70;
    // base
    DrawText(title.c_str(), tx, ty, 56, {0, 229, 255, 255});
    // glitch layers
    int gx = (int)(std::sin(GetTime() * 8.0) * 2.0);
    DrawText(title.c_str(), tx + gx, ty + 2, 56, {255, 0, 128, 60});
    DrawText(title.c_str(), tx - gx, ty - 2, 56, {0, 255, 156, 40});

    // Render UI on top
    // if reveal in progress, draw a scanline and a wipe effect
    if (menu_reveal_progress_ > 0.0f && menu_reveal_progress_ < 1.0f) {
        int screen_h = GetScreenHeight();
        // moving scanline y based on progress
        int line_y = (int)((1.0f - menu_reveal_progress_) * (screen_h + 200)) - 100;
        // draw bright scanline
        DrawRectangle(0, line_y, GetScreenWidth(), 4, {0, 229, 255, 120});
        // slight noise band
        DrawRectangle(0, line_y + 6, GetScreenWidth(), 2, {255, 255, 255, 10});
    }

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
        {20, 20, 30, 220},  // Normal - dark transparent
        {36, 36, 52, 230}, // Hovered
        {16, 16, 24, 200},  // Pressed
        {12, 12, 16, 180}     // Disabled
    );
    play_button->set_text_color({220, 240, 255, 255}, {160, 160, 160, 255});
    play_button->set_font_size(24);
    play_button->set_neon({0, 229, 255, 255}, {0, 229, 255, 100});
    play_button->set_glitch_params(2.2f, 8.0f, true);
    play_button->set_on_click([this]() { on_play_clicked(); });
    ui_manager_.add_component("play_button", play_button);

    // Settings button
    auto settings_button = std::make_shared<UIButton>(center_x - 100, center_y + 10, 200, 40, "SETTINGS");
    settings_button->set_colors(
        {20, 20, 30, 200},  // Normal
        {36, 36, 52, 220}, // Hovered
        {16, 16, 24, 200},  // Pressed
        {12, 12, 16, 160}     // Disabled
    );
    settings_button->set_text_color({200, 230, 255, 220}, {140, 140, 140, 200});
    settings_button->set_font_size(18);
    settings_button->set_neon({0, 229, 255, 220}, {0, 229, 255, 80});
    settings_button->set_glitch_params(1.6f, 6.0f, true);
    settings_button->set_on_click([this]() { on_settings_clicked(); });
    ui_manager_.add_component("settings_button", settings_button);

    // Quit button
    auto quit_button = std::make_shared<UIButton>(center_x - 100, center_y + 60, 200, 40, "QUIT");
    quit_button->set_colors(
        {30, 12, 12, 200},   // Normal
        {60, 18, 18, 220}, // Hovered
        {20, 8, 8, 200},   // Pressed
        {12, 6, 6, 160}     // Disabled
    );
    quit_button->set_text_color({255, 160, 160, 220}, {140, 80, 80, 200});
    quit_button->set_font_size(18);
    quit_button->set_neon({255, 80, 80, 220}, {255, 80, 80, 80});
    quit_button->set_glitch_params(1.8f, 7.0f, true);
    quit_button->set_on_click([this]() { on_quit_clicked(); });
    ui_manager_.add_component("quit_button", quit_button);

}

void MainMenuState::cleanup_ui() {
    ui_manager_.clear_components();
}

void MainMenuState::on_play_clicked() {
    if (state_manager_) {
        state_manager_->change_state("Lobby");
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