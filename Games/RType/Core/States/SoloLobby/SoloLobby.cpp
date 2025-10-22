/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Solo Lobby Game State Implementation
*/

#include "SoloLobby.hpp"
#include "Core/States/GameStateManager.hpp"
#include "UI/Components/GlitchButton.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Zipper.hpp"
#include <iostream>
#include <raylib.h>
#include <cmath>

void SoloLobbyState::enter() {
    std::cout << "[SoloLobby] Entering state" << std::endl;

    // Register all component types
    ui_registry_.register_component<UI::UIComponent>();
    ui_registry_.register_component<RType::UISoloLobbyPanel>();
    ui_registry_.register_component<RType::UISoloLobbyTitle>();
    ui_registry_.register_component<RType::UIStartSoloButton>();
    ui_registry_.register_component<RType::UISoloLobbyBackButton>();

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

void SoloLobbyState::exit() {
    std::cout << "[SoloLobby] Exiting state" << std::endl;
    cleanup_ui();
    initialized_ = false;
}

void SoloLobbyState::pause() {
    std::cout << "[SoloLobby] Pausing state" << std::endl;
}

void SoloLobbyState::resume() {
    std::cout << "[SoloLobby] Resuming state" << std::endl;
}

void SoloLobbyState::update(float delta_time) {
    if (!initialized_) return;

    // Update UI system
    ui_system_.update(ui_registry_, delta_time);

    // Update reveal animation
    update_reveal_animation(delta_time);

    // Update ASCII background
    ascii_timer_ += delta_time;
    if (ascii_timer_ >= ascii_interval_) {
        ascii_timer_ = 0.0f;
        // Randomly update some characters
        for (int i = 0; i < 5; ++i) {
            int row = ascii_rng_() % ascii_rows_;
            int col = ascii_rng_() % ascii_cols_;
            ascii_grid_[row][col] = ascii_charset_[ascii_rng_() % ascii_charset_.size()];
        }
    }
}

void SoloLobbyState::render() {
    if (!initialized_) return;

    // Render ASCII background
    render_ascii_background();

    // Render UI
    ui_system_.render(ui_registry_);
}

void SoloLobbyState::handle_input() {
    if (!initialized_) return;

    // Handle UI input
    ui_system_.process_input(ui_registry_);
}

void SoloLobbyState::setup_ui() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    float center_x = screen_width / 2.0f;
    float center_y = screen_height / 2.0f;

    // Create panel
    auto panel_entity = ui_registry_.spawn_entity();
    auto panel = std::make_shared<UI::UIPanel>(
        *PanelBuilder()
            .at(center_x - 200, center_y - 150)
            .size(400, 300)
            .backgroundColor({20, 25, 35, 220})
            .border(2.0f, {0, 229, 255, 180})
            .build(SCREEN_WIDTH, SCREEN_HEIGHT)
    );
    panel->_style.setHasShadow(true);
    panel->_style.setShadowColor({0, 0, 0, 200});
    panel->_style.setShadowOffset(5.0f);
    ui_registry_.add_component(panel_entity, UI::UIComponent(panel));
    ui_registry_.add_component(panel_entity, RType::UISoloLobbyPanel{});

    // Create title text
    auto title_entity = ui_registry_.spawn_entity();
    auto title = std::shared_ptr<UI::UIText>(
        TextBuilder()
            .at(center_x, center_y - 80)
            .text("SOLO MODE")
            .fontSize(48)
            .textColor({0, 229, 255, 255})
            .alignment(UI::TextAlignment::Center)
            .build(SCREEN_WIDTH, SCREEN_HEIGHT)
    );
    title->_style.setHasShadow(true);
    title->_style.setShadowColor({0, 150, 200, 180});
    title->_style.setShadowOffset({4.0f, 4.0f});
    ui_registry_.add_component(title_entity, UI::UIComponent(title));
    ui_registry_.add_component(title_entity, RType::UISoloLobbyTitle{});

    // Create start solo button
    auto start_entity = ui_registry_.spawn_entity();
    auto start_button = std::make_shared<RType::GlitchButton>(center_x - 120, center_y - 10, 240, 55, "START GAME");
    start_button->_style.setNormalColor({20, 30, 20, 220});
    start_button->_style.setHoveredColor({36, 52, 36, 240});
    start_button->_style.setPressedColor({16, 24, 16, 200});
    start_button->_style.setTextColor({220, 255, 220, 255});
    start_button->_style.setFontSize(24);
    start_button->set_neon_colors({0, 255, 100, 255}, {0, 255, 100, 120});
    start_button->set_glitch_params(2.5f, 8.5f, true);
    start_button->setOnClick([this]() { on_start_solo_clicked(); });
    ui_registry_.add_component(start_entity, UI::UIComponent(start_button));
    ui_registry_.add_component(start_entity, RType::UIStartSoloButton{});

    // Create back button
    auto back_entity = ui_registry_.spawn_entity();
    auto back_button = std::make_shared<RType::GlitchButton>(center_x - 120, center_y + 60, 240, 45, "BACK");
    back_button->_style.setNormalColor({30, 20, 20, 200});
    back_button->_style.setHoveredColor({52, 36, 36, 220});
    back_button->_style.setTextColor({255, 220, 220, 220});
    back_button->_style.setFontSize(20);
    back_button->set_neon_colors({255, 100, 100, 220}, {255, 100, 100, 100});
    back_button->set_glitch_params(2.0f, 7.5f, true);
    back_button->setOnClick([this]() { on_back_clicked(); });
    ui_registry_.add_component(back_entity, UI::UIComponent(back_button));
    ui_registry_.add_component(back_entity, RType::UISoloLobbyBackButton{});

    // Set all invisible initially
    auto* ui_components = ui_registry_.get_if<UI::UIComponent>();
    if (ui_components) {
        for (auto [comp, ent] : zipper(*ui_components)) {
            if (comp._ui_element) {
                comp._ui_element->setVisible(false);
            }
        }
    }
}

void SoloLobbyState::cleanup_ui() {
    // Collect all entity IDs first to avoid modifying while iterating
    std::vector<entity> entities_to_cleanup;

    auto* ui_components = ui_registry_.get_if<UI::UIComponent>();
    if (ui_components) {
        for (auto [comp, ent] : zipper(*ui_components)) {
            entities_to_cleanup.push_back(entity(ent));
        }
    }

    // Now remove all components from collected entities
    for (auto ent : entities_to_cleanup) {
        ui_registry_.remove_component<UI::UIComponent>(ent);
        ui_registry_.remove_component<RType::UISoloLobbyPanel>(ent);
        ui_registry_.remove_component<RType::UISoloLobbyTitle>(ent);
        ui_registry_.remove_component<RType::UIStartSoloButton>(ent);
        ui_registry_.remove_component<RType::UISoloLobbyBackButton>(ent);
    }
}

void SoloLobbyState::update_reveal_animation(float delta_time) {
    menu_elapsed_ += delta_time;

    if (menu_elapsed_ >= menu_show_delay_) {
        float reveal_time = menu_elapsed_ - menu_show_delay_;
        menu_reveal_progress_ = std::min(reveal_time / menu_reveal_duration_, 1.0f);

        // Ease-in-out animation
        float t = menu_reveal_progress_;
        t = t < 0.5 ? 2 * t * t : 1 - std::pow(-2 * t + 2, 2) / 2;

        // Update UI element visibility and positions
        auto* ui_components = ui_registry_.get_if<UI::UIComponent>();
        if (ui_components) {
            for (auto [comp, ent] : zipper(*ui_components)) {
                if (comp._ui_element) {
                    comp._ui_element->setVisible(true);
                    // Add subtle animation if needed
                }
            }
        }
    }
}

void SoloLobbyState::render_ascii_background() {
    int font_size = ascii_font_size_;
    Color text_color = {0, 229, 255, 120};

    for (int row = 0; row < ascii_rows_; ++row) {
        for (int col = 0; col < ascii_cols_; ++col) {
            char c = ascii_grid_[row][col];
            if (c != ' ') {
                DrawText(std::string(1, c).c_str(),
                        col * (font_size / 2), row * font_size,
                        font_size, text_color);
            }
        }
    }
}

void SoloLobbyState::on_start_solo_clicked() {
    std::cout << "[SoloLobby] Start Solo button clicked" << std::endl;
    if (state_manager_) {
        state_manager_->change_state("SimpleGame");
    }
}

void SoloLobbyState::on_back_clicked() {
    std::cout << "[SoloLobby] Back button clicked" << std::endl;
    if (state_manager_) {
        state_manager_->change_state("MainMenu");
    }
}