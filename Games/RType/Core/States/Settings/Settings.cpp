/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Settings Game State Implementation
*/

#include "Settings.hpp"
#include "Core/States/GameStateManager.hpp"
#include "../../UI/Components/GlitchButton.hpp"
#include "ECS/Zipper.hpp"
#include <iostream>
#include <raylib.h>

SettingsState::SettingsState(Application* app) : app_(app) {
}

void SettingsState::enter() {
    std::cout << "[Settings] Entering state" << std::endl;

    // Register all component types
    ui_registry_.register_component<UI::UIComponent>();
    ui_registry_.register_component<RType::UISettingsPanel>();
    ui_registry_.register_component<RType::UISettingsTitle>();
    ui_registry_.register_component<RType::UINameLabel>();
    ui_registry_.register_component<RType::UINameInput>();
    ui_registry_.register_component<RType::UIAudioLabel>();
    ui_registry_.register_component<RType::UIGraphicsLabel>();
    ui_registry_.register_component<RType::UIBackButton>();

    // Prepare ascii background
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    ascii_font_size_ = 12;
    ascii_cols_ = std::max(10, sw / (ascii_font_size_ / 2));
    ascii_rows_ = std::max(8, sh / ascii_font_size_);
    ascii_grid_.assign(ascii_rows_, std::string(ascii_cols_, ' '));

    setup_ui();
    initialized_ = true;
}

void SettingsState::exit() {
    std::cout << "[Settings] Exiting state" << std::endl;
    cleanup_ui();
    initialized_ = false;
}

void SettingsState::pause() {
    std::cout << "[Settings] Pausing state" << std::endl;
    // Hide UI when paused
    auto* ui_components = ui_registry_.get_if<UI::UIComponent>();
    if (ui_components) {
        for (auto& comp : *ui_components) {
            if (comp._ui_element) {
                comp._ui_element->set_visible(false);
            }
        }
    }
}

void SettingsState::resume() {
    std::cout << "[Settings] Resuming state" << std::endl;
    // Show UI when resumed
    auto* ui_components = ui_registry_.get_if<UI::UIComponent>();
    if (ui_components) {
        for (auto& comp : *ui_components) {
            if (comp._ui_element) {
                comp._ui_element->set_visible(true);
            }
        }
    }
}

void SettingsState::update(float delta_time) {
    if (!initialized_) return;

    // Update UI system
    ui_system_.update(ui_registry_, delta_time);

    // Update ascii background
    ascii_timer_ += delta_time;
    if (ascii_timer_ >= ascii_interval_) {
        ascii_timer_ = 0.0f;
        std::uniform_int_distribution<int> row_dist(0, ascii_rows_ - 1);
        std::uniform_int_distribution<int> col_dist(0, ascii_cols_ - 1);
        std::uniform_int_distribution<int> char_dist(0, (int)ascii_charset_.size() - 1);
        int changes = std::max(1, (ascii_cols_ * ascii_rows_) / 50);
        for (int i = 0; i < changes; ++i) {
            int r = row_dist(ascii_rng_);
            int c = col_dist(ascii_rng_);
            ascii_grid_[r][c] = ascii_charset_[char_dist(ascii_rng_)];
        }
    }
}

void SettingsState::render() {
    if (!initialized_) return;

    // Draw ascii background
    ClearBackground({10, 10, 12, 255});
    Color ascii_color = {0, 229, 255, 90};
    int start_x = 10;
    int start_y = 30;
    for (int r = 0; r < ascii_rows_; ++r) {
        for (int c = 0; c < ascii_cols_; ++c) {
            char ch = ascii_grid_[r][c];
            if (ch == ' ') continue;
            int x = start_x + c * (ascii_font_size_ / 2);
            int y = start_y + r * ascii_font_size_;
            DrawText(std::string(1, ch).c_str(), x, y, ascii_font_size_, ascii_color);
        }
    }

    // Translucent overlay
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 140});

    // Render UI via system
    ui_system_.render(ui_registry_);
}

void SettingsState::handle_input() {
    if (!initialized_) return;

    // Handle escape key to go back
    if (IsKeyPressed(KEY_ESCAPE)) {
        on_back_clicked();
        return;
    }

    ui_system_.process_input(ui_registry_);
}

void SettingsState::setup_ui() {
    // Get screen dimensions
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    float center_x = screen_width / 2.0f;
    float center_y = screen_height / 2.0f;

    // Settings panel
    auto panel_entity = ui_registry_.spawn_entity();
    auto settings_panel = std::make_shared<UI::UIPanel>(center_x - 200, center_y - 150, 400, 300);
    UI::PanelStyle panel_style;
    panel_style._background_color = {20, 25, 35, 240};
    panel_style._border_color = {0, 229, 255, 180};
    panel_style._border_thickness = 2.0f;
    panel_style._has_shadow = true;
    panel_style._shadow_color = {0, 0, 0, 200};
    panel_style._shadow_offset = 5.0f;
    settings_panel->set_style(panel_style);
    ui_registry_.add_component(panel_entity, UI::UIComponent(settings_panel));
    ui_registry_.add_component(panel_entity, RType::UISettingsPanel{});

    // Settings title
    auto title_entity = ui_registry_.spawn_entity();
    auto title = std::make_shared<UI::UIText>(center_x, center_y - 120, "SETTINGS");
    UI::TextStyle title_style;
    title_style._text_color = {0, 229, 255, 255};
    title_style._font_size = 32;
    title_style._alignment = UI::TextAlignment::Center;
    title_style._has_shadow = true;
    title_style._shadow_color = {0, 150, 200, 180};
    title_style._shadow_offset = {3.0f, 3.0f};
    title->set_style(title_style);
    ui_registry_.add_component(title_entity, UI::UIComponent(title));
    ui_registry_.add_component(title_entity, RType::UISettingsTitle{});

    // Player name label
    auto name_label_entity = ui_registry_.spawn_entity();
    auto name_label = std::make_shared<UI::UIText>(center_x - 180, center_y - 70, "Player Name:");
    UI::TextStyle label_style;
    label_style._text_color = {200, 200, 200, 255};
    label_style._font_size = 18;
    label_style._alignment = UI::TextAlignment::Left;
    name_label->set_style(label_style);
    ui_registry_.add_component(name_label_entity, UI::UIComponent(name_label));
    ui_registry_.add_component(name_label_entity, RType::UINameLabel{});

    // Player name input
    auto name_input_entity = ui_registry_.spawn_entity();
    auto name_input = std::make_shared<UI::UIInputField>(center_x - 180, center_y - 45, 360, 35, "Enter your name...");
    UI::InputFieldStyle input_style;
    input_style._background_color = {30, 35, 45, 255};
    input_style._focused_color = {40, 45, 60, 255};
    input_style._border_color = {80, 80, 120, 255};
    input_style._focused_border_color = {0, 229, 255, 255};
    input_style._text_color = {255, 255, 255, 255};
    input_style._placeholder_color = {150, 150, 150, 255};
    input_style._cursor_color = {0, 229, 255, 255};
    input_style._font_size = 18;
    input_style._border_thickness = 1.5f;
    name_input->set_style(input_style);
    name_input->set_max_length(20);
    name_input->set_on_text_changed([this](const std::string& name) { on_player_name_changed(name); });
    ui_registry_.add_component(name_input_entity, UI::UIComponent(name_input));
    ui_registry_.add_component(name_input_entity, RType::UINameInput{});

    // Audio label
    auto audio_label_entity = ui_registry_.spawn_entity();
    auto audio_label = std::make_shared<UI::UIText>(center_x - 180, center_y + 10, "Audio Volume: 100%");
    audio_label->set_style(label_style);
    ui_registry_.add_component(audio_label_entity, UI::UIComponent(audio_label));
    ui_registry_.add_component(audio_label_entity, RType::UIAudioLabel{});

    // Graphics label
    auto graphics_label_entity = ui_registry_.spawn_entity();
    auto graphics_label = std::make_shared<UI::UIText>(center_x - 180, center_y + 40, "Graphics: High");
    graphics_label->set_style(label_style);
    ui_registry_.add_component(graphics_label_entity, UI::UIComponent(graphics_label));
    ui_registry_.add_component(graphics_label_entity, RType::UIGraphicsLabel{});

    // Back button
    auto back_button_entity = ui_registry_.spawn_entity();
    auto back_button = std::make_shared<RType::GlitchButton>(center_x - 75, center_y + 90, 150, 40, "BACK");
    UI::ButtonStyle back_style;
    back_style._normal_color = {20, 20, 30, 220};
    back_style._hovered_color = {36, 36, 52, 240};
    back_style._pressed_color = {16, 16, 24, 200};
    back_style._text_color = {220, 240, 255, 255};
    back_style._font_size = 20;
    back_button->set_style(back_style);
    back_button->set_neon_colors({0, 229, 255, 220}, {0, 229, 255, 100});
    back_button->set_glitch_params(1.8f, 7.0f, true);
    back_button->set_on_click([this]() { on_back_clicked(); });
    ui_registry_.add_component(back_button_entity, UI::UIComponent(back_button));
    ui_registry_.add_component(back_button_entity, RType::UIBackButton{});
}

void SettingsState::cleanup_ui() {
    // Registry entities will be cleaned up automatically when state exits
}

void SettingsState::on_back_clicked() {
    std::cout << "[Settings] Back button clicked" << std::endl;
    if (state_manager_) {
        state_manager_->pop_state();
    }
}

void SettingsState::on_player_name_changed(const std::string& name) {
    std::cout << "[Settings] Player name changed: " << name << std::endl;
    // Store the player name in the application
    // if (app_ && !name.empty()) {
    //     app_->set_player_name(name);
    // }
}