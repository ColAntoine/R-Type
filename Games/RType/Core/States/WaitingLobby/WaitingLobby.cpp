/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Waiting Lobby State Implementation
*/

#include "WaitingLobby.hpp"
#include "Core/States/GameStateManager.hpp"
#include "Application.hpp"
#include <iostream>
#include <raylib.h>

WaitingLobbyState::WaitingLobbyState(Application* app) : app_(app) {
}

void WaitingLobbyState::enter() {
    // Set up event subscription for player list updates
    if (app_) {
        auto& event_manager = app_->get_event_manager();
        event_manager.subscribe<PlayerListEvent>([this](const PlayerListEvent& e) {
            handle_player_list_update(e);
        });
        event_manager.subscribe<StartGameEvent>([this](const StartGameEvent& e) {
            handle_start_game(e);
        });
    }

    setup_ui();
    initialized_ = true;

    // Add local player to the list (will be updated by server data)
    PlayerInfo local_player;
    local_player.player_id = app_ ? app_->get_local_player_id() : 1;
    local_player.name = "Player " + std::to_string(local_player.player_id);
    local_player.is_ready = false;
    local_player.is_local_player = true;
    connected_players_.clear();
    connected_players_.push_back(local_player);

    update_player_list();
}

void WaitingLobbyState::exit() {
    cleanup_ui();
    initialized_ = false;
    connected_players_.clear();
}

void WaitingLobbyState::pause() {
    ui_manager_.set_all_visible(false);
}

void WaitingLobbyState::resume() {
    ui_manager_.set_all_visible(true);
}

void WaitingLobbyState::update(float delta_time) {
    if (!initialized_) return;

    // Update UI manager (important for input handling)
    ui_manager_.update(delta_time);

    // Update player list and ready status periodically
    update_ready_status();
}

void WaitingLobbyState::render() {
    if (!initialized_) return;
    ui_manager_.render();
}

void WaitingLobbyState::handle_input() {
    if (!initialized_) return;
    ui_manager_.handle_input();
}

void WaitingLobbyState::setup_ui() {
    int center_x = 1024 / 2;
    int center_y = 768 / 2;

    // Title
    auto title = std::make_shared<UIText>(center_x - 100, 50, "WAITING LOBBY", 36);
    title->set_text_color({255, 255, 255, 255});
    ui_manager_.add_component("title", title);

    // Status text
    auto status_text = std::make_shared<UIText>(center_x - 150, 100, "Waiting for other players...", 24);
    status_text->set_text_color({200, 200, 200, 255});
    ui_manager_.add_component("status_text", status_text);

    // Player list panel
    auto player_panel = std::make_shared<UIPanel>(center_x - 200, 150, 400, 300);
    player_panel->set_background_color({40, 40, 40, 200});
    player_panel->set_border_color({60, 60, 60, 255});
    ui_manager_.add_component("player_panel", player_panel);

    // Player list title
    auto player_list_title = std::make_shared<UIText>(center_x - 180, 170, "Connected Players:", 20);
    player_list_title->set_text_color({255, 255, 255, 255});
    ui_manager_.add_component("player_list_title", player_list_title);

    // Ready button
    auto ready_button = std::make_shared<UIButton>(center_x - 100, center_y + 150, 200, 50, "READY");
    ready_button->set_colors(
        {70, 180, 70, 255},   // Normal - Green
        {100, 210, 100, 255}, // Hovered
        {50, 160, 50, 255},   // Pressed
        {40, 40, 40, 255}     // Disabled
    );
    ready_button->set_on_click([this]() { on_ready_clicked(); });
    ui_manager_.add_component("ready_button", ready_button);

    // Back button
    auto back_button = std::make_shared<UIButton>(50, 700, 150, 40, "DISCONNECT");
    back_button->set_colors(
        {180, 70, 70, 255},   // Normal - Red
        {210, 100, 100, 255}, // Hovered
        {160, 50, 50, 255},   // Pressed
        {40, 40, 40, 255}     // Disabled
    );
    back_button->set_on_click([this]() { on_back_clicked(); });
    ui_manager_.add_component("back_button", back_button);

    // Game start status
    auto game_status = std::make_shared<UIText>(center_x - 150, center_y + 220, "", 18);
    game_status->set_text_color({255, 255, 100, 255});
    ui_manager_.add_component("game_status", game_status);
}

void WaitingLobbyState::cleanup_ui() {
    ui_manager_.clear_components();
}

void WaitingLobbyState::update_player_list() {
    // Remove existing player list items
    for (int i = 0; i < 8; ++i) {
        ui_manager_.remove_component("player_" + std::to_string(i));
    }

    // Add current players to UI
    int y_offset = 200;
    for (size_t i = 0; i < connected_players_.size() && i < 8; ++i) {
        const auto& player = connected_players_[i];

        std::string player_text = player.name;

        auto player_label = std::make_shared<UIText>(1024/2 - 180, y_offset + (i * 25), player_text, 16);

        // Color code based on ready status
        if (player.is_ready) {
            player_label->set_text_color({100, 255, 100, 255}); // Green for ready
        } else {
            player_label->set_text_color({255, 200, 100, 255}); // Orange for not ready
        }

        ui_manager_.add_component("player_" + std::to_string(i), player_label);
    }
}

void WaitingLobbyState::update_ready_status() {
    // Count ready players
    int ready_count = 0;
    int total_count = connected_players_.size();

    for (const auto& player : connected_players_) {
        if (player.is_ready) {
            ready_count++;
        }
    }

    // Update game status text
    auto game_status = ui_manager_.get_component<UIText>("game_status");
    if (game_status) {
        if (ready_count == total_count && total_count > 0) {
            game_status->set_text("All players ready! Starting game...");
            game_status->set_text_color({100, 255, 100, 255}); // Green
        } else {
            game_status->set_text("Players ready: " + std::to_string(ready_count) + "/" + std::to_string(total_count));
            game_status->set_text_color({255, 255, 100, 255}); // Yellow
        }
    }
}

void WaitingLobbyState::update_ready_button() {
    auto ready_button = ui_manager_.get_component<UIButton>("ready_button");
    if (!ready_button) return;

    // Find local player
    for (const auto& player : connected_players_) {
        if (player.is_local_player) {
            // Button text should indicate what will happen when clicked
            ready_button->set_text(player.is_ready ? "NOT READY" : "READY");
            break;
        }
    }
}

void WaitingLobbyState::on_ready_clicked() {
    // Find local player and toggle ready status
    for (auto& player : connected_players_) {
        if (player.is_local_player) {
            player.is_ready = !player.is_ready;

            // Send ready status to server through Application
            if (app_) {
                app_->send_ready_signal(player.is_ready);
            }
            update_ready_button();
            break;
        }
    }

    update_player_list();
    update_ready_status();
}

void WaitingLobbyState::on_back_clicked() {
    // TODO: Disconnect from server
    if (app_) {
        // app_->disconnect_from_server();
    }

    if (state_manager_) {
        state_manager_->change_state("Lobby"); // Go back to connection screen
    }
}

void WaitingLobbyState::handle_player_list_update(const PlayerListEvent& event) {
    // Replace current player list with server data
    connected_players_.clear();

    for (const auto& server_player : event.players) {
        PlayerInfo player;
        player.player_id = server_player.player_id;
        player.name = server_player.name;
        player.is_ready = server_player.is_ready;
        player.is_local_player = server_player.is_local_player;

        connected_players_.push_back(player);
    }

    // Update UI with new player list
    update_player_list();
    update_ready_status();
    update_ready_button();  // Ensure button text is consistent
}

void WaitingLobbyState::handle_start_game(const StartGameEvent& event) {
    // Transition to InGame state when all players are ready
    if (state_manager_) {
        state_manager_->change_state("InGame");
    }
}