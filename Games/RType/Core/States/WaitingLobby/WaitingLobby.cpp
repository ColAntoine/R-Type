/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Waiting Lobby State Implementation
*/

#include "WaitingLobby.hpp"
#include "Core/States/GameStateManager.hpp"
#include "../../UI/Components/GlitchButton.hpp"
#include "Application.hpp"
#include "ECS/Zipper.hpp"
#include <iostream>
#include <raylib.h>

WaitingLobbyState::WaitingLobbyState(Application* app) : app_(app) {
}

void WaitingLobbyState::enter() {
    std::cout << "[WaitingLobby] Entering state" << std::endl;

    // Register all component types
    ui_registry_.register_component<UI::UIComponent>();
    ui_registry_.register_component<RType::UIWaitingPanel>();
    ui_registry_.register_component<RType::UIWaitingTitle>();
    ui_registry_.register_component<RType::UIWaitingStatus>();
    ui_registry_.register_component<RType::UIPlayerListTitle>();
    ui_registry_.register_component<RType::UIPlayerSlot>();
    ui_registry_.register_component<RType::UIReadyButton>();
    ui_registry_.register_component<RType::UIDisconnectButton>();
    ui_registry_.register_component<RType::UIGameStatus>();

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

    // Add local player to the list (will be updated by server data)
    PlayerInfo local_player;
    local_player.player_id = app_ ? app_->get_local_player_id() : 1;
    local_player.name = "Player " + std::to_string(local_player.player_id);
    local_player.is_ready = false;
    local_player.is_local_player = true;
    connected_players_.clear();
    connected_players_.push_back(local_player);

    // Prepare ascii background
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    ascii_font_size_ = 12;
    ascii_cols_ = std::max(10, sw / (ascii_font_size_ / 2));
    ascii_rows_ = std::max(8, sh / ascii_font_size_);
    ascii_grid_.assign(ascii_rows_, std::string(ascii_cols_, ' '));

    setup_ui();
    update_player_list();
    
    initialized_ = true;
}

void WaitingLobbyState::exit() {
    std::cout << "[WaitingLobby] Exiting state" << std::endl;
    cleanup_ui();
    initialized_ = false;
    connected_players_.clear();
}

void WaitingLobbyState::pause() {
    std::cout << "[WaitingLobby] Pausing state" << std::endl;
}

void WaitingLobbyState::resume() {
    std::cout << "[WaitingLobby] Resuming state" << std::endl;
}

void WaitingLobbyState::update(float delta_time) {
    if (!initialized_) return;

    // Update UI system
    ui_system_.update(ui_registry_, delta_time);

    // Update player list and ready status periodically
    update_ready_status();

    // Update ASCII background animation
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

void WaitingLobbyState::render() {
    if (!initialized_) return;
    
    // Draw background
    ClearBackground({10, 10, 12, 255});
    
    // Draw ASCII background
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

    ui_system_.render(ui_registry_);
}

void WaitingLobbyState::handle_input() {
    if (!initialized_) return;
    ui_system_.process_input(ui_registry_);
}

void WaitingLobbyState::setup_ui() {
    int center_x = 1024 / 2;
    int center_y = 768 / 2;

    // Panel
    auto panel_entity = ui_registry_.spawn_entity();
    auto panel = std::make_shared<UI::UIPanel>(center_x - 300, 30, 600, 660);
    UI::PanelStyle panel_style;
    panel_style._background_color = {20, 20, 25, 230};
    panel_style._border_color = {0, 229, 255, 255};
    panel_style._border_thickness = 2.0f;
    panel_style._has_shadow = true;
    panel_style._shadow_color = {0, 0, 0, 150};
    panel_style._shadow_offset = {4.0f};
    panel->set_style(panel_style);
    ui_registry_.add_component(panel_entity, UI::UIComponent(panel));
    ui_registry_.add_component(panel_entity, RType::UIWaitingPanel{});

    // Title
    auto title_entity = ui_registry_.spawn_entity();
    auto title = std::make_shared<UI::UIText>(center_x, 70, "WAITING LOBBY");
    UI::TextStyle title_style;
    title_style._text_color = {0, 229, 255, 255};
    title_style._font_size = 40;
    title_style._alignment = UI::TextAlignment::Center;
    title_style._has_shadow = true;
    title_style._shadow_color = {0, 180, 200, 180};
    title_style._shadow_offset = {3.0f, 3.0f};
    title->set_style(title_style);
    ui_registry_.add_component(title_entity, UI::UIComponent(title));
    ui_registry_.add_component(title_entity, RType::UIWaitingTitle{});

    // Status text
    auto status_entity = ui_registry_.spawn_entity();
    auto status_text = std::make_shared<UI::UIText>(center_x - 180, 130, "Waiting for other players...");
    UI::TextStyle status_style;
    status_style._text_color = {200, 200, 200, 255};
    status_style._font_size = 22;
    status_text->set_style(status_style);
    ui_registry_.add_component(status_entity, UI::UIComponent(status_text));
    ui_registry_.add_component(status_entity, RType::UIWaitingStatus{});

    // Player list title
    auto list_title_entity = ui_registry_.spawn_entity();
    auto list_title = std::make_shared<UI::UIText>(center_x - 280, 180, "Connected Players:");
    UI::TextStyle list_title_style;
    list_title_style._text_color = {0, 229, 255, 255};
    list_title_style._font_size = 24;
    list_title->set_style(list_title_style);
    ui_registry_.add_component(list_title_entity, UI::UIComponent(list_title));
    ui_registry_.add_component(list_title_entity, RType::UIPlayerListTitle{});

    // Create 8 player slot entities
    int y_offset = 220;
    for (int i = 0; i < 8; ++i) {
        auto slot_entity = ui_registry_.spawn_entity();
        auto slot_text = std::make_shared<UI::UIText>(center_x - 280, y_offset + (i * 35), "---");
        UI::TextStyle slot_style;
        slot_style._text_color = {100, 100, 100, 255};
        slot_style._font_size = 18;
        slot_text->set_style(slot_style);
        ui_registry_.add_component(slot_entity, UI::UIComponent(slot_text));
        
        // Create the tag with designated initialization
        RType::UIPlayerSlot slot_tag{};
        slot_tag.slot_index = i;
        ui_registry_.add_component(slot_entity, std::move(slot_tag));
    }

    // Ready button (using GlitchButton)
    auto ready_btn_entity = ui_registry_.spawn_entity();
    auto ready_button = std::make_shared<RType::GlitchButton>(center_x - 120, center_y + 230, 240, 55, "READY");
    UI::ButtonStyle ready_style;
    ready_style._normal_color = {20, 30, 20, 220};
    ready_style._hovered_color = {30, 50, 30, 240};
    ready_style._pressed_color = {15, 25, 15, 200};
    ready_style._text_color = {220, 255, 220, 255};
    ready_style._font_size = 26;
    ready_button->set_style(ready_style);
    ready_button->set_neon_colors({100, 255, 100, 255}, {80, 200, 80, 120});
    ready_button->set_glitch_params(0.03f, 8.0f, 0.5f);
    ready_button->set_on_click([this]() { on_ready_clicked(); });
    ui_registry_.add_component(ready_btn_entity, UI::UIComponent(ready_button));
    ui_registry_.add_component(ready_btn_entity, RType::UIReadyButton{});

    // Disconnect button
    auto disconnect_btn_entity = ui_registry_.spawn_entity();
    auto disconnect_button = std::make_shared<RType::GlitchButton>(40, 680, 200, 50, "DISCONNECT");
    UI::ButtonStyle disconnect_style;
    disconnect_style._normal_color = {40, 15, 15, 220};
    disconnect_style._hovered_color = {70, 20, 20, 240};
    disconnect_style._pressed_color = {30, 10, 10, 200};
    disconnect_style._text_color = {255, 180, 180, 255};
    disconnect_style._font_size = 22;
    disconnect_button->set_style(disconnect_style);
    disconnect_button->set_neon_colors({255, 100, 100, 255}, {200, 80, 80, 120});
    disconnect_button->set_glitch_params(0.03f, 8.0f, 0.5f);
    disconnect_button->set_on_click([this]() { on_back_clicked(); });
    ui_registry_.add_component(disconnect_btn_entity, UI::UIComponent(disconnect_button));
    ui_registry_.add_component(disconnect_btn_entity, RType::UIDisconnectButton{});

    // Game status text
    auto game_status_entity = ui_registry_.spawn_entity();
    auto game_status = std::make_shared<UI::UIText>(center_x - 180, center_y + 310, "");
    UI::TextStyle status_text_style;
    status_text_style._text_color = {255, 255, 100, 255};
    status_text_style._font_size = 20;
    game_status->set_style(status_text_style);
    ui_registry_.add_component(game_status_entity, UI::UIComponent(game_status));
    ui_registry_.add_component(game_status_entity, RType::UIGameStatus{});
}

void WaitingLobbyState::cleanup_ui() {
    // Registry cleanup happens automatically
}

void WaitingLobbyState::update_player_list() {
    // Use zipper to iterate through all player slot components
    auto* ui_components = ui_registry_.get_if<UI::UIComponent>();
    auto* player_slots = ui_registry_.get_if<RType::UIPlayerSlot>();
    
    if (!ui_components || !player_slots) return;

    for (auto [ui_comp, slot, ent] : zipper(*ui_components, *player_slots)) {
        auto text_comp = std::dynamic_pointer_cast<UI::UIText>(ui_comp._ui_element);
        if (!text_comp) continue;

        int slot_index = slot.slot_index;

        // Update text based on connected players
        if (slot_index < (int)connected_players_.size()) {
            const auto& player = connected_players_[slot_index];
            text_comp->set_text(player.name);
            
            // Color code based on ready status
            UI::TextStyle style;
            style._font_size = 18;
            if (player.is_ready) {
                style._text_color = {100, 255, 100, 255}; // Green for ready
            } else {
                style._text_color = {255, 200, 100, 255}; // Orange for not ready
            }
            text_comp->set_style(style);
        } else {
            // Empty slot
            text_comp->set_text("---");
            UI::TextStyle style;
            style._font_size = 18;
            style._text_color = {100, 100, 100, 255}; // Gray
            text_comp->set_style(style);
        }
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

    // Use zipper to find and update game status text
    auto* ui_components = ui_registry_.get_if<UI::UIComponent>();
    auto* game_statuses = ui_registry_.get_if<RType::UIGameStatus>();
    
    if (!ui_components || !game_statuses) return;

    for (auto [ui_comp, status_tag, ent] : zipper(*ui_components, *game_statuses)) {
        auto text_comp = std::dynamic_pointer_cast<UI::UIText>(ui_comp._ui_element);
        if (!text_comp) continue;

        UI::TextStyle style;
        style._font_size = 20;
        if (ready_count == total_count && total_count > 0) {
            text_comp->set_text("All players ready! Starting game...");
            style._text_color = {100, 255, 100, 255}; // Green
        } else {
            text_comp->set_text("Players ready: " + std::to_string(ready_count) + "/" + std::to_string(total_count));
            style._text_color = {255, 255, 100, 255}; // Yellow
        }
        text_comp->set_style(style);
    }
}

void WaitingLobbyState::update_ready_button() {
    // Use zipper to find and update ready button
    auto* ui_components = ui_registry_.get_if<UI::UIComponent>();
    auto* ready_buttons = ui_registry_.get_if<RType::UIReadyButton>();
    
    if (!ui_components || !ready_buttons) return;

    // Find local player
    for (const auto& player : connected_players_) {
        if (player.is_local_player) {
            // Update button text
            for (auto [ui_comp, btn_tag, ent] : zipper(*ui_components, *ready_buttons)) {
                auto button_comp = std::dynamic_pointer_cast<UI::UIButton>(ui_comp._ui_element);
                if (!button_comp) continue;

                // Button text should indicate what will happen when clicked
                button_comp->set_text(player.is_ready ? "NOT READY" : "READY");
            }
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

void WaitingLobbyState::handle_start_game(__attribute_maybe_unused__ const StartGameEvent& event) {
    // Transition to InGame state when all players are ready
    if (state_manager_) {
        state_manager_->change_state("InGame");
    }
}