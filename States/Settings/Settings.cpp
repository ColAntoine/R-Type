/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Settings Game State Implementation
*/

#include "Settings.hpp"
#include "Core/States/GameStateManager.hpp"
#include "../../UI/Components/GlitchButton.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Zipper.hpp"
#include <iostream>
#include <raylib.h>

SettingsState::SettingsState(Application* app) : app_(app) {
}

void SettingsState::enter() {
    std::cout << "[Settings] Entering state" << std::endl;

    // Register all component types
    _uiRegistry.register_component<UI::UIComponent>();
    _uiRegistry.register_component<RType::UISettingsPanel>();
    _uiRegistry.register_component<RType::UISettingsTitle>();
    _uiRegistry.register_component<RType::UINameLabel>();
    _uiRegistry.register_component<RType::UINameInput>();
    _uiRegistry.register_component<RType::UIAudioLabel>();
    _uiRegistry.register_component<RType::UIGraphicsLabel>();
    _uiRegistry.register_component<RType::UIBackButton>();

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
    auto* ui_components = _uiRegistry.get_if<UI::UIComponent>();
    if (ui_components) {
        for (auto& comp : *ui_components) {
            if (comp._ui_element) {
                comp._ui_element->setVisible(false);
            }
        }
    }
}

void SettingsState::resume() {
    std::cout << "[Settings] Resuming state" << std::endl;
    // Show UI when resumed
    auto* ui_components = _uiRegistry.get_if<UI::UIComponent>();
    if (ui_components) {
        for (auto& comp : *ui_components) {
            if (comp._ui_element) {
                comp._ui_element->setVisible(true);
            }
        }
    }
}

void SettingsState::update(float delta_time) {
    if (!initialized_) return;

    // Update UI system
    ui_system_.update(_uiRegistry, delta_time);

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
    ui_system_.render(_uiRegistry);
}

void SettingsState::handle_input() {
    if (!initialized_) return;

    // Handle escape key to go back
    if (IsKeyPressed(KEY_ESCAPE)) {
        on_back_clicked();
        return;
    }

    ui_system_.process_input(_uiRegistry);
}

void SettingsState::setup_ui() {
    // Get screen dimensions
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    float center_x = screen_width / 2.0f;
    float center_y = screen_height / 2.0f;

    // Settings panel
    auto panel_entity = ui_registry_.spawn_entity();
    auto settings_panel = std::make_shared<UI::UIPanel>(
        *PanelBuilder()
            .at(center_x - 200, center_y - 150)
            .size(400, 300)
            .backgroundColor({20, 25, 35, 240})
            .border(2.0f, {0, 229, 255, 180})
            .build(SCREEN_WIDTH, SCREEN_HEIGHT)
    );
    settings_panel->_style.setHasShadow(true);
    settings_panel->_style.setShadowColor({0, 0, 0, 200});
    settings_panel->_style.setShadowOffset(5.0f);
    ui_registry_.add_component(panel_entity, UI::UIComponent(settings_panel));
    ui_registry_.add_component(panel_entity, RType::UISettingsPanel{});

    // Settings title
    auto title_entity = ui_registry_.spawn_entity();
    auto title = std::shared_ptr<UI::UIText>(
        TextBuilder()
            .at(center_x, center_y - 120)
            .text("SETTINGS")
            .fontSize(32)
            .textColor({0, 229, 255, 255})
            .alignment(UI::TextAlignment::Center)
            .build(SCREEN_WIDTH, SCREEN_HEIGHT)
    );
    title->_style.setHasShadow(true);
    title->_style.setShadowColor({0, 150, 200, 180});
    title->_style.setShadowOffset({3.0f, 3.0f});
    ui_registry_.add_component(title_entity, UI::UIComponent(title));
    ui_registry_.add_component(title_entity, RType::UISettingsTitle{});

    // Player name label
    auto name_label_entity = ui_registry_.spawn_entity();
    auto name_label = std::shared_ptr<UI::UIText>(
        TextBuilder()
            .at(center_x - 180, center_y - 70)
            .text("Player Name:")
            .fontSize(18)
            .textColor({200, 200, 200, 255})
            .alignment(UI::TextAlignment::Left)
            .build(SCREEN_WIDTH, SCREEN_HEIGHT)
    );
    ui_registry_.add_component(name_label_entity, UI::UIComponent(name_label));
    ui_registry_.add_component(name_label_entity, RType::UINameLabel{});

    // Player name input
    auto name_input_entity = ui_registry_.spawn_entity();
    auto name_input = std::shared_ptr<UI::UIInputField>(
        InputBuilder()
            .at(center_x - 180, center_y - 45)
            .size(360, 35)
            .placeholder("Enter your name...")
            .backgroundColor({30, 35, 45, 255})
            .focusedColor({40, 45, 60, 255})
            .border(1.5f, {80, 80, 120, 255})
            .focusedBorderColor({0, 229, 255, 255})
            .textColor({255, 255, 255, 255})
            .placeholderColor({150, 150, 150, 255})
            .cursorColor({0, 229, 255, 255})
            .fontSize(18)
            .build(SCREEN_WIDTH, SCREEN_HEIGHT)
    );
    name_input->setMaxLength(20);
    name_input->setOnTextChanged([this](const std::string& name) { on_player_name_changed(name); });
    ui_registry_.add_component(name_input_entity, UI::UIComponent(name_input));
    ui_registry_.add_component(name_input_entity, RType::UINameInput{});

    // Audio label
    auto audio_label_entity = ui_registry_.spawn_entity();
    auto audio_label = std::shared_ptr<UI::UIText>(
        TextBuilder()
            .at(center_x - 180, center_y + 10)
            .text("Audio Volume: 100%")
            .fontSize(18)
            .textColor({200, 200, 200, 255})
            .alignment(UI::TextAlignment::Left)
            .build(SCREEN_WIDTH, SCREEN_HEIGHT)
    );
    ui_registry_.add_component(audio_label_entity, UI::UIComponent(audio_label));
    ui_registry_.add_component(audio_label_entity, RType::UIAudioLabel{});

    // Graphics label
    auto graphics_label_entity = ui_registry_.spawn_entity();
    auto graphics_label = std::shared_ptr<UI::UIText>(
        TextBuilder()
            .at(center_x - 180, center_y + 40)
            .text("Graphics: High")
            .fontSize(18)
            .textColor({200, 200, 200, 255})
            .alignment(UI::TextAlignment::Left)
            .build(SCREEN_WIDTH, SCREEN_HEIGHT)
    );
    ui_registry_.add_component(graphics_label_entity, UI::UIComponent(graphics_label));
    ui_registry_.add_component(graphics_label_entity, RType::UIGraphicsLabel{});

    // Back button
    auto back_button_entity = _uiRegistry.spawn_entity();
    auto back_button = std::make_shared<RType::GlitchButton>(center_x - 75, center_y + 90, 150, 40, "BACK");
    back_button->_style.setNormalColor({20, 20, 30, 220});
    back_button->_style.setHoveredColor({36, 36, 52, 240});
    back_button->_style.setPressedColor({16, 16, 24, 200});
    back_button->_style.setTextColor({220, 240, 255, 255});
    back_button->_style.setFontSize(20);
    back_button->set_neon_colors({0, 229, 255, 220}, {0, 229, 255, 100});
    back_button->set_glitch_params(1.8f, 7.0f, true);
<<<<<<< HEAD:States/Settings/Settings.cpp
    back_button->set_on_click([this]() { on_back_clicked(); });
    _uiRegistry.add_component(back_button_entity, UI::UIComponent(back_button));
    _uiRegistry.add_component(back_button_entity, RType::UIBackButton{});
=======
    back_button->setOnClick([this]() { on_back_clicked(); });
    ui_registry_.add_component(back_button_entity, UI::UIComponent(back_button));
    ui_registry_.add_component(back_button_entity, RType::UIBackButton{});
>>>>>>> main:Games/RType/Core/States/Settings/Settings.cpp
}

void SettingsState::cleanup_ui() {
    // Registry entities will be cleaned up automatically when state exits
}

void SettingsState::on_back_clicked() {
    std::cout << "[Settings] Back button clicked" << std::endl;
    if (_stateManager) {
        _stateManager->pop_state();
    }
}

void SettingsState::on_player_name_changed(const std::string& name) {
    std::cout << "[Settings] Player name changed: " << name << std::endl;
    // Store the player name in the application
    // if (app_ && !name.empty()) {
    //     app_->set_player_name(name);
    // }
}