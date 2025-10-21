/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Lobby Game State Implementation
*/

#include "Lobby.hpp"
#include "Core/States/GameStateManager.hpp"
#include "../../UI/Components/GlitchButton.hpp"
#include "ECS/UI/UIBuilder.hpp"
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
    auto panel_entity = ui_registry_.spawn_entity();
    auto lobby_panel = std::shared_ptr<UI::UIPanel>(
        PanelBuilder()
            .at(center_x - 200, center_y - 150)
            .size(400, 320)
            .backgroundColor({20, 25, 35, 220})
            .border(2.0f, {0, 229, 255, 180})
            .build(SCREEN_WIDTH, SCREEN_HEIGHT)
    );
    lobby_panel->_style.setHasShadow(true);
    lobby_panel->_style.setShadowColor({0, 0, 0, 200});
    lobby_panel->_style.setShadowOffset(5.0f);
    ui_registry_.add_component(panel_entity, UI::UIComponent(lobby_panel));
    ui_registry_.add_component(panel_entity, RType::UILobbyPanel{});

    // Title
    auto title_entity = ui_registry_.spawn_entity();
    auto title = std::shared_ptr<UI::UIText>(
        TextBuilder()
            .at(center_x, center_y - 120)
            .text("SERVER LOBBY")
            .fontSize(36)
            .textColor({0, 229, 255, 255})
            .alignment(UI::TextAlignment::Center)
            .build(SCREEN_WIDTH, SCREEN_HEIGHT)
    );
    title->_style.setHasShadow(true);
    title->_style.setShadowColor({0, 150, 200, 180});
    title->_style.setShadowOffset({3.0f, 3.0f});
    ui_registry_.add_component(title_entity, UI::UIComponent(title));
    ui_registry_.add_component(title_entity, RType::UILobbyTitle{});

    // Server IP label
    auto ip_label_entity = ui_registry_.spawn_entity();
    auto ip_label = std::shared_ptr<UI::UIText>(
        TextBuilder()
            .at(center_x - 180, center_y - 70)
            .text("Server IP:")
            .fontSize(18)
            .textColor({200, 200, 200, 255})
            .alignment(UI::TextAlignment::Left)
            .build(SCREEN_WIDTH, SCREEN_HEIGHT)
    );
    ui_registry_.add_component(ip_label_entity, UI::UIComponent(ip_label));
    ui_registry_.add_component(ip_label_entity, RType::UIIPLabel{});

    // Server IP input
    auto ip_input_entity = ui_registry_.spawn_entity();
    auto ip_input = std::shared_ptr<UI::UIInputField>(
        InputBuilder()
            .at(center_x - 180, center_y - 45)
            .size(240, 35)
            .placeholder("127.0.0.1")
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
    ip_input->setText(server_ip_);
    ip_input->setOnTextChanged([this](const std::string& ip) { on_server_ip_changed(ip); });
    ui_registry_.add_component(ip_input_entity, UI::UIComponent(ip_input));
    ui_registry_.add_component(ip_input_entity, RType::UIIPInput{});

    // Server Port label
    auto port_label_entity = ui_registry_.spawn_entity();
    auto port_label = std::make_shared<UI::UIText>(center_x + 70, center_y - 70, "Port:", 18, Color{200, 200, 200, 255});
    ui_registry_.add_component(port_label_entity, UI::UIComponent(port_label));
    ui_registry_.add_component(port_label_entity, RType::UIPortLabel{});

    // Server Port input
    auto port_input_entity = ui_registry_.spawn_entity();
    auto port_input = std::shared_ptr<UI::UIInputField>(
        InputBuilder()
            .at(center_x + 70, center_y - 45)
            .size(110, 35)
            .placeholder("8080")
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
    port_input->setText(std::to_string(server_port_));
    port_input->setMaxLength(5);
    port_input->setOnTextChanged([this](const std::string& port) { on_server_port_changed(port); });
    ui_registry_.add_component(port_input_entity, UI::UIComponent(port_input));
    ui_registry_.add_component(port_input_entity, RType::UIPortInput{});

    // Connect button
    auto connect_entity = _uiRegistry.spawn_entity();
    auto connect_button = std::make_shared<RType::GlitchButton>(center_x - 100, center_y + 20, 200, 50, "CONNECT");
    connect_button->_style.setNormalColor({20, 80, 20, 220});
    connect_button->_style.setHoveredColor({30, 120, 30, 240});
    connect_button->_style.setPressedColor({15, 60, 15, 200});
    connect_button->_style.setTextColor({200, 255, 200, 255});
    connect_button->_style.setFontSize(24);
    connect_button->set_neon_colors({100, 255, 100, 255}, {100, 255, 100, 120});
    connect_button->set_glitch_params(2.2f, 8.0f, true);
    connect_button->setOnClick([this]() { on_connect_clicked(); });
    ui_registry_.add_component(connect_entity, UI::UIComponent(connect_button));
    ui_registry_.add_component(connect_entity, RType::UIConnectButton{});

    // Back button
    auto back_entity = _uiRegistry.spawn_entity();
    auto back_button = std::make_shared<RType::GlitchButton>(center_x - 100, center_y + 90, 200, 40, "BACK");
    back_button->_style.setNormalColor({20, 20, 30, 220});
    back_button->_style.setHoveredColor({36, 36, 52, 240});
    back_button->_style.setPressedColor({16, 16, 24, 200});
    back_button->_style.setTextColor({220, 240, 255, 255});
    back_button->_style.setFontSize(20);
    back_button->set_neon_colors({0, 229, 255, 220}, {0, 229, 255, 100});
    back_button->set_glitch_params(1.8f, 7.0f, true);
    back_button->setOnClick([this]() { on_back_clicked(); });
    ui_registry_.add_component(back_entity, UI::UIComponent(back_button));
    ui_registry_.add_component(back_entity, RType::UILobbyBackButton{});

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