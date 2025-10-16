/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Lobby Game State Implementation
*/

#include "Lobby.hpp"
#include "Core/States/GameStateManager.hpp"
#include <iostream>
#include <raylib.h>
#include <random>

// ASCII background for Lobby
static std::vector<std::string> lobby_ascii_grid;
static int lobby_ascii_cols = 0;
static int lobby_ascii_rows = 0;
static int lobby_ascii_font_size = 12;
static float lobby_ascii_timer = 0.0f;
static float lobby_ascii_interval = 0.04f;
static std::mt19937 lobby_ascii_rng((unsigned)time(nullptr));
static std::string lobby_ascii_charset = " .,:;i!lI|/\\()1{}[]?-_+~<>^*abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

LobbyState::LobbyState() {}

void LobbyState::enter() {
    std::cout << "Entering Lobby State" << std::endl;
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    lobby_ascii_font_size = 12;
    lobby_ascii_cols = std::max(10, sw / (lobby_ascii_font_size / 2));
    lobby_ascii_rows = std::max(8, sh / lobby_ascii_font_size);
    lobby_ascii_grid.assign(lobby_ascii_rows, std::string(lobby_ascii_cols, ' '));

    setup_ui();
    initialized_ = true;
}

void LobbyState::exit() {
    std::cout << "Exiting Lobby State" << std::endl;
    cleanup_ui();
    initialized_ = false;
}

void LobbyState::pause() {
    std::cout << "Pausing Lobby State" << std::endl;
    ui_manager_.set_all_visible(false);
}

void LobbyState::resume() {
    std::cout << "Resuming Lobby State" << std::endl;
    ui_manager_.set_all_visible(true);
}

void LobbyState::update(float delta_time) {
    if (!initialized_) return;
    ui_manager_.update(delta_time);

    lobby_ascii_timer += delta_time;
    if (lobby_ascii_timer >= lobby_ascii_interval) {
        lobby_ascii_timer = 0.0f;
        std::uniform_int_distribution<int> row_dist(0, lobby_ascii_rows - 1);
        std::uniform_int_distribution<int> col_dist(0, lobby_ascii_cols - 1);
        std::uniform_int_distribution<int> char_dist(0, (int)lobby_ascii_charset.size() - 1);
        int changes = std::max(1, (lobby_ascii_cols * lobby_ascii_rows) / 60);
        for (int i = 0; i < changes; ++i) {
            int r = row_dist(lobby_ascii_rng);
            int c = col_dist(lobby_ascii_rng);
            lobby_ascii_grid[r][c] = lobby_ascii_charset[char_dist(lobby_ascii_rng)];
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
    for (int r = 0; r < lobby_ascii_rows; ++r) {
        for (int c = 0; c < lobby_ascii_cols; ++c) {
            char ch = lobby_ascii_grid[r][c];
            if (ch == ' ') continue;
            int x = start_x + c * (lobby_ascii_font_size / 2);
            int y = start_y + r * lobby_ascii_font_size;
            DrawText(std::string(1, ch).c_str(), x, y, lobby_ascii_font_size, ascii_color);
        }
    }

    // Title
    DrawText("SERVER LOBBY", GetScreenWidth()/2 - 140, 80, 40, {0, 229, 255, 200});

    // Render UI
    ui_manager_.render();
}

void LobbyState::handle_input() {
    if (!initialized_) return;

    // Handle escape key to go back
    if (IsKeyPressed(KEY_ESCAPE)) {
        on_back_clicked();
        return;
    }

    ui_manager_.handle_input();
}

void LobbyState::setup_ui() {
    // Get screen dimensions
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();

    float center_x = screen_width / 2.0f;
    float center_y = screen_height / 2.0f;

    // Main lobby panel
    auto lobby_panel = std::make_shared<UIPanel>(center_x - 200, center_y - 100, 400, 280);
    lobby_panel->set_background_color({40, 40, 60, 220});
    lobby_panel->set_border_color({100, 100, 150, 255});
    ui_manager_.add_component("lobby_panel", lobby_panel);

    // Server IP section
    auto ip_label = std::make_shared<UIText>(center_x - 180, center_y - 70, "Server IP:", 20);
    ip_label->set_text_color({200, 200, 200, 255});
    ui_manager_.add_component("ip_label", ip_label);

    auto ip_input = std::make_shared<UIInputField>(center_x - 180, center_y - 45, 200, 35, "127.0.0.1");
    ip_input->set_text(server_ip_);
    ip_input->set_colors(
        {60, 60, 80, 255},    // Background
        {80, 80, 100, 255},   // Focused background
        {100, 100, 150, 255}, // Border
        {150, 150, 255, 255}  // Focused border
    );
    ip_input->set_on_text_changed([this](const std::string& ip) { on_server_ip_changed(ip); });
    ui_manager_.add_component("ip_input", ip_input);

    // Server Port section
    auto port_label = std::make_shared<UIText>(center_x + 40, center_y - 70, "Port:", 20);
    port_label->set_text_color({200, 200, 200, 255});
    ui_manager_.add_component("port_label", port_label);

    auto port_input = std::make_shared<UIInputField>(center_x + 40, center_y - 45, 100, 35, "8080");
    port_input->set_text(std::to_string(server_port_));
    port_input->set_colors(
        {60, 60, 80, 255},    // Background
        {80, 80, 100, 255},   // Focused background
        {100, 100, 150, 255}, // Border
        {150, 150, 255, 255}  // Focused border
    );
    port_input->set_on_text_changed([this](const std::string& port) { on_server_port_changed(port); });
    ui_manager_.add_component("port_input", port_input);

    // Connection status
    auto status_label = std::make_shared<UIText>(center_x - 180, center_y + 10, "Status: Ready to connect", 18);
    status_label->set_text_color({150, 200, 150, 255});
    ui_manager_.add_component("status_label", status_label);

    // Connect button
    auto connect_button = std::make_shared<UIButton>(center_x - 100, center_y + 50, 200, 50, "CONNECT");
    connect_button->set_colors(
        {70, 180, 70, 255},   // Normal - Green
        {100, 210, 100, 255}, // Hovered
        {50, 160, 50, 255},   // Pressed
        {40, 40, 40, 255}     // Disabled
    );
    connect_button->set_on_click([this]() { on_connect_clicked(); });
    ui_manager_.add_component("connect_button", connect_button);

    // Back button
    auto back_button = std::make_shared<UIButton>(center_x - 170, center_y + 120, 100, 40, "BACK");
    back_button->set_colors(
        {100, 100, 100, 255}, // Normal
        {130, 130, 130, 255}, // Hovered
        {80, 80, 80, 255},    // Pressed
        {40, 40, 40, 255}     // Disabled
    );
    back_button->set_on_click([this]() { on_back_clicked(); });
    ui_manager_.add_component("back_button", back_button);

    std::cout << "Lobby UI setup complete" << std::endl;
}

void LobbyState::cleanup_ui() {
    ui_manager_.clear_components();
}

void LobbyState::on_connect_clicked() {
    // Update status
    auto status_label = ui_manager_.get_component<UIText>("status_label");
    if (status_label) {
        status_label->set_text("Status: Connecting...");
        status_label->set_text_color({255, 255, 100, 255}); // Yellow
    }
}

void LobbyState::on_back_clicked() {
    if (state_manager_) {
        state_manager_->change_state("MainMenu");
    }
}

void LobbyState::on_server_ip_changed(const std::string& ip) {
    server_ip_ = ip;
    std::cout << "Server IP changed to: " << server_ip_ << std::endl;
}

void LobbyState::on_server_port_changed(const std::string& port) {
    try {
        server_port_ = std::stoi(port);
        std::cout << "Server port changed to: " << server_port_ << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Invalid port number: " << port << std::endl;
    }
}