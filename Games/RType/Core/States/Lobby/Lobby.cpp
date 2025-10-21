/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Lobby Game State Implementation
*/

#include "Lobby.hpp"
#include "Core/States/GameStateManager.hpp"
#include "../../UI/Components/GlitchButton.hpp"
#include "ECS/Zipper.hpp"
#include <iostream>
#include <raylib.h>
#include <random>

LobbyState::LobbyState() {}

void LobbyState::enter() {
    std::cout << "[Lobby] Entering state" << std::endl;

    // Register all component types
    _uiRegistry.register_component<UI::UIComponent>();
    _uiRegistry.register_component<RType::UILobbyPanel>();
    _uiRegistry.register_component<RType::UILobbyTitle>();
    _uiRegistry.register_component<RType::UIIPLabel>();
    _uiRegistry.register_component<RType::UIIPInput>();
    _uiRegistry.register_component<RType::UIPortLabel>();
    _uiRegistry.register_component<RType::UIPortInput>();
    _uiRegistry.register_component<RType::UIConnectButton>();
    _uiRegistry.register_component<RType::UILobbyBackButton>();

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

void LobbyState::exit() {
    std::cout << "[Lobby] Exiting state" << std::endl;
    cleanup_ui();
    initialized_ = false;
}

void LobbyState::pause() {
    std::cout << "[Lobby] Pausing state" << std::endl;
}

void LobbyState::resume() {
    std::cout << "[Lobby] Resuming state" << std::endl;
}

void LobbyState::update(float delta_time) {
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
        int changes = std::max(1, (ascii_cols_ * ascii_rows_) / 60);
        for (int i = 0; i < changes; ++i) {
            int r = row_dist(ascii_rng_);
            int c = col_dist(ascii_rng_);
            ascii_grid_[r][c] = ascii_charset_[char_dist(ascii_rng_)];
        }
    }
}

void LobbyState::render() {
    if (!initialized_) return;

    // ASCII background
    ClearBackground({8, 10, 12, 255});
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

    // Dark overlay
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 120});

    // Render UI via system
    ui_system_.render(_uiRegistry);
}

void LobbyState::handle_input() {
    if (!initialized_) return;

    // Handle escape key to go back
    if (IsKeyPressed(KEY_ESCAPE)) {
        on_back_clicked();
        return;
    }

    ui_system_.process_input(_uiRegistry);
}

void LobbyState::setup_ui() {
    // Get screen dimensions
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    float center_x = screen_width / 2.0f;
    float center_y = screen_height / 2.0f;

    // Main lobby panel
    auto panel_entity = _uiRegistry.spawn_entity();
    auto lobby_panel = std::make_shared<UI::UIPanel>(center_x - 200, center_y - 150, 400, 320);
    UI::PanelStyle panel_style;
    panel_style._background_color = {20, 25, 35, 220};
    panel_style._border_color = {0, 229, 255, 180};
    panel_style._border_thickness = 2.0f;
    panel_style._has_shadow = true;
    panel_style._shadow_color = {0, 0, 0, 200};
    panel_style._shadow_offset = 5.0f;
    lobby_panel->set_style(panel_style);
    _uiRegistry.add_component(panel_entity, UI::UIComponent(lobby_panel));
    _uiRegistry.add_component(panel_entity, RType::UILobbyPanel{});

    // Title
    auto title_entity = _uiRegistry.spawn_entity();
    auto title = std::make_shared<UI::UIText>(center_x, center_y - 120, "SERVER LOBBY");
    UI::TextStyle title_style;
    title_style._text_color = {0, 229, 255, 255};
    title_style._font_size = 36;
    title_style._alignment = UI::TextAlignment::Center;
    title_style._has_shadow = true;
    title_style._shadow_color = {0, 150, 200, 180};
    title_style._shadow_offset = {3.0f, 3.0f};
    title->set_style(title_style);
    _uiRegistry.add_component(title_entity, UI::UIComponent(title));
    _uiRegistry.add_component(title_entity, RType::UILobbyTitle{});

    // Server IP label
    auto ip_label_entity = _uiRegistry.spawn_entity();
    auto ip_label = std::make_shared<UI::UIText>(center_x - 180, center_y - 70, "Server IP:");
    UI::TextStyle label_style;
    label_style._text_color = {200, 200, 200, 255};
    label_style._font_size = 18;
    label_style._alignment = UI::TextAlignment::Left;
    ip_label->set_style(label_style);
    _uiRegistry.add_component(ip_label_entity, UI::UIComponent(ip_label));
    _uiRegistry.add_component(ip_label_entity, RType::UIIPLabel{});

    // Server IP input
    auto ip_input_entity = _uiRegistry.spawn_entity();
    auto ip_input = std::make_shared<UI::UIInputField>(center_x - 180, center_y - 45, 240, 35, "127.0.0.1");
    ip_input->set_text(server_ip_);
    UI::InputFieldStyle ip_input_style;
    ip_input_style._background_color = {30, 35, 45, 255};
    ip_input_style._focused_color = {40, 45, 60, 255};
    ip_input_style._border_color = {80, 80, 120, 255};
    ip_input_style._focused_border_color = {0, 229, 255, 255};
    ip_input_style._text_color = {255, 255, 255, 255};
    ip_input_style._placeholder_color = {150, 150, 150, 255};
    ip_input_style._cursor_color = {0, 229, 255, 255};
    ip_input_style._font_size = 18;
    ip_input_style._border_thickness = 1.5f;
    ip_input->set_style(ip_input_style);
    ip_input->set_on_text_changed([this](const std::string& ip) { on_server_ip_changed(ip); });
    _uiRegistry.add_component(ip_input_entity, UI::UIComponent(ip_input));
    _uiRegistry.add_component(ip_input_entity, RType::UIIPInput{});

    // Server Port label
    auto port_label_entity = _uiRegistry.spawn_entity();
    auto port_label = std::make_shared<UI::UIText>(center_x + 70, center_y - 70, "Port:");
    port_label->set_style(label_style);
    _uiRegistry.add_component(port_label_entity, UI::UIComponent(port_label));
    _uiRegistry.add_component(port_label_entity, RType::UIPortLabel{});

    // Server Port input
    auto port_input_entity = _uiRegistry.spawn_entity();
    auto port_input = std::make_shared<UI::UIInputField>(center_x + 70, center_y - 45, 110, 35, "8080");
    port_input->set_text(std::to_string(server_port_));
    port_input->set_style(ip_input_style);
    port_input->set_max_length(5);
    port_input->set_on_text_changed([this](const std::string& port) { on_server_port_changed(port); });
    _uiRegistry.add_component(port_input_entity, UI::UIComponent(port_input));
    _uiRegistry.add_component(port_input_entity, RType::UIPortInput{});

    // Connect button
    auto connect_entity = _uiRegistry.spawn_entity();
    auto connect_button = std::make_shared<RType::GlitchButton>(center_x - 100, center_y + 20, 200, 50, "CONNECT");
    UI::ButtonStyle connect_style;
    connect_style._normal_color = {20, 80, 20, 220};
    connect_style._hovered_color = {30, 120, 30, 240};
    connect_style._pressed_color = {15, 60, 15, 200};
    connect_style._text_color = {200, 255, 200, 255};
    connect_style._font_size = 24;
    connect_button->set_style(connect_style);
    connect_button->set_neon_colors({100, 255, 100, 255}, {100, 255, 100, 120});
    connect_button->set_glitch_params(2.2f, 8.0f, true);
    connect_button->set_on_click([this]() { on_connect_clicked(); });
    _uiRegistry.add_component(connect_entity, UI::UIComponent(connect_button));
    _uiRegistry.add_component(connect_entity, RType::UIConnectButton{});

    // Back button
    auto back_entity = _uiRegistry.spawn_entity();
    auto back_button = std::make_shared<RType::GlitchButton>(center_x - 100, center_y + 90, 200, 40, "BACK");
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
    _uiRegistry.add_component(back_entity, UI::UIComponent(back_button));
    _uiRegistry.add_component(back_entity, RType::UILobbyBackButton{});

    std::cout << "[Lobby] UI setup complete" << std::endl;
}

void LobbyState::cleanup_ui() {
    // Registry entities will be cleaned up automatically
}

void LobbyState::on_connect_clicked() {
    // Update status
}

void LobbyState::on_back_clicked() {
    std::cout << "[Lobby] Back button clicked" << std::endl;
    if (_stateManager) {
        _stateManager->change_state("MainMenu");
    }
}

void LobbyState::on_server_ip_changed(const std::string& ip) {
    server_ip_ = ip;
    std::cout << "[Lobby] Server IP changed to: " << server_ip_ << std::endl;
}

void LobbyState::on_server_port_changed(const std::string& port) {
    try {
        server_port_ = std::stoi(port);
        std::cout << "[Lobby] Server port changed to: " << server_port_ << std::endl;
    } catch (const std::exception& e) {
        std::cout << "[Lobby] Invalid port number: " << port << std::endl;
    }
}