/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Main Menu Game State Implementation
*/

#include "MainMenu.hpp"
#include "Core/States/GameStateManager.hpp"
#include "UI/Components/GlitchButton.hpp"
#include "ECS/Zipper.hpp"
#include <iostream>
#include <raylib.h>
#include <cmath>

void MainMenuState::enter() {
    std::cout << "[MainMenu] Entering state" << std::endl;

    // Register all component types
    _registry.register_component<UI::UIComponent>();
    _registry.register_component<RType::UIMainPanel>();
    _registry.register_component<RType::UITitleText>();
    _registry.register_component<RType::UIPlayButton>();
    _registry.register_component<RType::UISettingsButton>();
    _registry.register_component<RType::UIQuitButton>();

    // Prepare ascii grid
    ascii_font_size_ = 12;
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int font_w = std::max(1, ascii_font_size_ / 2);
    int font_h = ascii_font_size_;
    ascii_cols_ = std::max(10, (sw + font_w - 1) / font_w);
    ascii_rows_ = std::max(8, (sh + font_h - 1) / font_h);
    ascii_grid_.assign(ascii_rows_, std::string(ascii_cols_, ' '));

    setup_ui();

    // Start with reveal animation
    menu_elapsed_ = 0.0f;
    menu_reveal_progress_ = 0.0f;

    initialized_ = true;
}

void MainMenuState::exit() {
    std::cout << "[MainMenu] Exiting state" << std::endl;
    cleanup_ui();
    initialized_ = false;
}

void MainMenuState::pause() {
    std::cout << "[MainMenu] Pausing state" << std::endl;
}

void MainMenuState::resume() {
    std::cout << "[MainMenu] Resuming state" << std::endl;
}

void MainMenuState::update(float delta_time) {
    if (!initialized_) return;

    // Update UI system
    ui_system_.update(_registry, delta_time);

    // Update ascii background
    ascii_timer_ += delta_time;
    if (ascii_timer_ >= ascii_interval_) {
        ascii_timer_ = 0.0f;
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

    // Update reveal animation
    update_reveal_animation(delta_time);
}

void MainMenuState::update_reveal_animation(float delta_time) {
    menu_elapsed_ += delta_time;

    if (menu_elapsed_ >= menu_show_delay_) {
        float t = menu_elapsed_ - menu_show_delay_;
        menu_reveal_progress_ = std::min(1.0f, t / menu_reveal_duration_);

        // Use zipper to update visibility based on component tags
        auto* ui_components = _registry.get_if<UI::UIComponent>();
        auto* main_panels = _registry.get_if<RType::UIMainPanel>();
        auto* title_texts = _registry.get_if<RType::UITitleText>();
        auto* play_buttons = _registry.get_if<RType::UIPlayButton>();
        auto* settings_buttons = _registry.get_if<RType::UISettingsButton>();
        auto* quit_buttons = _registry.get_if<RType::UIQuitButton>();

        if (!ui_components) return;

        // Panel and title appear immediately
        if (main_panels) {
            for (auto [ui_comp, panel_tag, ent] : zipper(*ui_components, *main_panels)) {
                if (ui_comp._ui_element) {
                    ui_comp._ui_element->set_visible(menu_reveal_progress_ >= 0.0f);
                }
            }
        }

        if (title_texts) {
            for (auto [ui_comp, title_tag, ent] : zipper(*ui_components, *title_texts)) {
                if (ui_comp._ui_element) {
                    ui_comp._ui_element->set_visible(menu_reveal_progress_ >= 0.0f);
                }
            }
        }

        // Play button at 33%
        if (play_buttons) {
            for (auto [ui_comp, play_tag, ent] : zipper(*ui_components, *play_buttons)) {
                if (ui_comp._ui_element) {
                    ui_comp._ui_element->set_visible(menu_reveal_progress_ >= 0.33f);
                }
            }
        }

        // Settings button at 66%
        if (settings_buttons) {
            for (auto [ui_comp, settings_tag, ent] : zipper(*ui_components, *settings_buttons)) {
                if (ui_comp._ui_element) {
                    ui_comp._ui_element->set_visible(menu_reveal_progress_ >= 0.66f);
                }
            }
        }

        // Quit button at 100%
        if (quit_buttons) {
            for (auto [ui_comp, quit_tag, ent] : zipper(*ui_components, *quit_buttons)) {
                if (ui_comp._ui_element) {
                    ui_comp._ui_element->set_visible(menu_reveal_progress_ >= 0.99f);
                }
            }
        }
    }
}

void MainMenuState::render() {
    if (!initialized_) return;

    // Dark background
    ClearBackground({8, 10, 12, 255});

    // ASCII glitch background
    int start_x = 10;
    int start_y = 30;
    Color ascii_color = {0, 229, 255, 90};
    for (int r = 0; r < ascii_rows_; ++r) {
        for (int c = 0; c < ascii_cols_; ++c) {
            char ch = ascii_grid_[r][c];
            if (ch == ' ') continue;
            int x = start_x + c * (ascii_font_size_ / 2);
            int y = start_y + r * ascii_font_size_;
            DrawText(std::string(1, ch).c_str(), x, y, ascii_font_size_, ascii_color);
        }
    }

    // Dark translucent overlay
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 120});

    // Render UI via system
    ui_system_.render(_registry);

    // Scanline effect during reveal
    if (menu_reveal_progress_ > 0.0f && menu_reveal_progress_ < 1.0f) {
        int screen_h = GetScreenHeight();
        int line_y = (int)((1.0f - menu_reveal_progress_) * (screen_h + 200)) - 100;
        DrawRectangle(0, line_y, GetScreenWidth(), 4, {0, 229, 255, 120});
        DrawRectangle(0, line_y + 6, GetScreenWidth(), 2, {255, 255, 255, 10});
    }
}

void MainMenuState::handle_input() {
    if (!initialized_) return;
    ui_system_.process_input(_registry);
}

void MainMenuState::setup_ui() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    float center_x = screen_width / 2.0f;
    float center_y = screen_height / 2.0f;

    // Create panel
    auto panel_entity = _registry.spawn_entity();
    auto panel = std::make_shared<UI::UIPanel>(center_x - 175, center_y - 150, 350, 350);
    UI::PanelStyle panel_style;
    panel_style._background_color = {20, 25, 35, 220};
    panel_style._border_color = {0, 229, 255, 180};
    panel_style._border_thickness = 2.0f;
    panel_style._has_shadow = true;
    panel_style._shadow_color = {0, 0, 0, 200};
    panel_style._shadow_offset = {5.0f};
    panel->set_style(panel_style);
    _registry.add_component(panel_entity, UI::UIComponent(panel));
    _registry.add_component(panel_entity, RType::UIMainPanel{});

    // Create title text
    auto title_entity = _registry.spawn_entity();
    auto title = std::make_shared<UI::UIText>(center_x, center_y - 100, "R-TYPE");
    UI::TextStyle title_style;
    title_style._text_color = {0, 229, 255, 255};
    title_style._font_size = 52;
    title_style._alignment = UI::TextAlignment::Center;
    title_style._has_shadow = true;
    title_style._shadow_color = {0, 150, 200, 180};
    title_style._shadow_offset = {4.0f, 4.0f};
    title->set_style(title_style);
    _registry.add_component(title_entity, UI::UIComponent(title));
    _registry.add_component(title_entity, RType::UITitleText{});

    // Create play button
    auto play_entity = _registry.spawn_entity();
    auto play_button = std::make_shared<RType::GlitchButton>(center_x - 120, center_y - 20, 240, 55, "PLAY");
    UI::ButtonStyle play_style;
    play_style._normal_color = {20, 20, 30, 220};
    play_style._hovered_color = {36, 36, 52, 240};
    play_style._pressed_color = {16, 16, 24, 200};
    play_style._text_color = {220, 240, 255, 255};
    play_style._font_size = 28;
    play_button->set_style(play_style);
    play_button->set_neon_colors({0, 229, 255, 255}, {0, 229, 255, 120});
    play_button->set_glitch_params(2.5f, 8.5f, true);
    play_button->set_on_click([this]() { on_play_clicked(); });
    _registry.add_component(play_entity, UI::UIComponent(play_button));
    _registry.add_component(play_entity, RType::UIPlayButton{});

    // Create settings button
    auto settings_entity = _registry.spawn_entity();
    auto settings_button = std::make_shared<RType::GlitchButton>(center_x - 120, center_y + 50, 240, 45, "SETTINGS");
    UI::ButtonStyle settings_style;
    settings_style._normal_color = {20, 20, 30, 200};
    settings_style._hovered_color = {36, 36, 52, 220};
    settings_style._text_color = {200, 230, 255, 220};
    settings_style._font_size = 20;
    settings_button->set_style(settings_style);
    settings_button->set_neon_colors({0, 229, 255, 220}, {0, 229, 255, 100});
    settings_button->set_glitch_params(1.8f, 7.0f, true);
    settings_button->set_on_click([this]() { on_settings_clicked(); });
    _registry.add_component(settings_entity, UI::UIComponent(settings_button));
    _registry.add_component(settings_entity, RType::UISettingsButton{});

    // Create quit button
    auto quit_entity = _registry.spawn_entity();
    auto quit_button = std::make_shared<RType::GlitchButton>(center_x - 120, center_y + 110, 240, 45, "QUIT");
    UI::ButtonStyle quit_style;
    quit_style._normal_color = {40, 15, 15, 200};
    quit_style._hovered_color = {70, 20, 20, 220};
    quit_style._text_color = {255, 180, 180, 220};
    quit_style._font_size = 20;
    quit_button->set_style(quit_style);
    quit_button->set_neon_colors({255, 100, 100, 220}, {255, 100, 100, 100});
    quit_button->set_glitch_params(2.0f, 7.5f, true);
    quit_button->set_on_click([this]() { on_quit_clicked(); });
    _registry.add_component(quit_entity, UI::UIComponent(quit_button));
    _registry.add_component(quit_entity, RType::UIQuitButton{});

    // Set all invisible initially
    auto* ui_components = _registry.get_if<UI::UIComponent>();
    if (ui_components) {
        for (auto [comp, ent] : zipper(*ui_components)) {
            if (comp._ui_element) {
                comp._ui_element->set_visible(false);
            }
        }
    }
}

void MainMenuState::cleanup_ui() {
    // Collect all entity IDs first to avoid modifying while iterating
    std::vector<entity> entities_to_cleanup;
    
    auto* ui_components = _registry.get_if<UI::UIComponent>();
    if (ui_components) {
        for (auto [comp, ent] : zipper(*ui_components)) {
            entities_to_cleanup.push_back(entity(ent));
        }
    }
    
    // Now remove all components from collected entities
    for (auto ent : entities_to_cleanup) {
        _registry.remove_component<UI::UIComponent>(ent);
        _registry.remove_component<RType::UIMainPanel>(ent);
        _registry.remove_component<RType::UITitleText>(ent);
        _registry.remove_component<RType::UIPlayButton>(ent);
        _registry.remove_component<RType::UISettingsButton>(ent);
        _registry.remove_component<RType::UIQuitButton>(ent);
    }
}

void MainMenuState::on_play_clicked() {
    std::cout << "[MainMenu] Play button clicked" << std::endl;
    if (_stateManager) {
        _stateManager->change_state("InGame");
    }
}

void MainMenuState::on_settings_clicked() {
    std::cout << "[MainMenu] Settings button clicked" << std::endl;
    if (_stateManager) {
        _stateManager->push_state("Settings");
    }
}

void MainMenuState::on_quit_clicked() {
    std::cout << "[MainMenu] Quit button clicked - closing game" << std::endl;
    if (_stateManager) {
        _stateManager->clear_states();
    }
}