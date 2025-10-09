/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** In-Game State Implementation
*/

#include "InGame.hpp"
#include "Application.hpp"
#include "Core/States/GameStateManager.hpp"
#include <iostream>
#include <raylib.h>

InGameState::InGameState(Application* app) : app_(app) {}

void InGameState::enter() {
    if (!app_) {
        std::cerr << "ERROR: Application pointer is null in InGameState::enter()" << std::endl;
        return;
    }

    setup_hud();
    initialized_ = true;
    paused_ = false;
}

void InGameState::exit() {
    cleanup_hud();
    initialized_ = false;
}

void InGameState::pause() {
    paused_ = true;
    ui_manager_.set_all_visible(false);
}

void InGameState::resume() {
    paused_ = false;
    ui_manager_.set_all_visible(true);
}

void InGameState::update(float delta_time) {
    if (!initialized_ || paused_) return;

    if (!app_) {
        std::cerr << "ERROR: Application pointer is null in InGameState::update()" << std::endl;
        return;
    }

    // Update UI
    ui_manager_.update(delta_time);
    update_hud();

    // Update existing game systems through Application
    app_->update_ecs_systems(delta_time);
    app_->update_traditional_ecs_systems(delta_time);
}

void InGameState::render() {
    if (!initialized_ || !app_) return;

    // Clear background
    ClearBackground({10, 10, 20, 255}); // Dark blue space background

    // The game entities are rendered automatically by the traditional ECS systems
    // (draw_system, sprite_system) when update_traditional_ecs_systems is called
    // We don't need to explicitly render them here as Raylib systems handle it

    // Render HUD on top
    ui_manager_.render();
}

void InGameState::handle_input() {
    if (!initialized_ || !app_) return;

    // Handle pause menu (ESC key)
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (state_manager_) {
            // Push pause menu state
            // state_manager_->push_state("Pause");
        }
    }

    // Handle UI input
    ui_manager_.handle_input();

    // Game input is handled by the InputSystem in the Application
}

void InGameState::setup_hud() {
    // Create HUD elements

    // FPS display
    auto fps_text = std::make_shared<UIText>(10, 10, "FPS: 0", 20);
    fps_text->set_text_color({255, 255, 0, 255}); // Yellow
    ui_manager_.add_component("fps_text", fps_text);

    // Player info
    auto player_info = std::make_shared<UIText>(10, 35, "Player: 0", 20);
    player_info->set_text_color({255, 255, 255, 255}); // White
    ui_manager_.add_component("player_info", player_info);

    // Connection status
    auto connection_status = std::make_shared<UIText>(10, 60, "Status: Disconnected", 20);
    connection_status->set_text_color({255, 100, 100, 255}); // Light red
    ui_manager_.add_component("connection_status", connection_status);

    // Position display
    auto position_text = std::make_shared<UIText>(10, 85, "Position: (0, 0)", 20);
    position_text->set_text_color({200, 200, 200, 255}); // Light gray
    ui_manager_.add_component("position_text", position_text);
}

void InGameState::cleanup_hud() {
    ui_manager_.clear_components();
}

void InGameState::update_hud() {
    if (!app_) {
        std::cerr << "ERROR: Application pointer is null in update_hud()" << std::endl;
        return;
    }

    try {
        auto& render_service = app_->get_service_manager().get_service<RenderService>();
        auto& network_service = app_->get_service_manager().get_service<NetworkService>();

        // Update FPS
        auto fps_text = ui_manager_.get_component<UIText>("fps_text");
        if (fps_text) {
            fps_text->set_text("FPS: " + std::to_string(render_service.get_fps()));
        }

        // Update player info
        auto player_info = ui_manager_.get_component<UIText>("player_info");
        if (player_info) {
            player_info->set_text("Player " + std::to_string(app_->get_local_player_id()));
        }

        // Update connection status
        auto connection_status = ui_manager_.get_component<UIText>("connection_status");
        if (connection_status) {
            if (network_service.is_connected()) {
                connection_status->set_text("Status: Connected");
                connection_status->set_text_color({100, 255, 100, 255}); // Green
            } else {
                connection_status->set_text("Status: Disconnected");
                connection_status->set_text_color({255, 100, 100, 255}); // Red
            }
        }

        // Update position
        auto position_text = ui_manager_.get_component<UIText>("position_text");
        if (position_text) {
            auto& ecs_registry = app_->get_ecs_registry();
            if (auto* pos_arr = ecs_registry.get_if<position>();
                pos_arr && pos_arr->size() > static_cast<size_t>(app_->get_local_player())) {
                auto& player_pos = (*pos_arr)[static_cast<size_t>(app_->get_local_player())];
                position_text->set_text("Position: (" + std::to_string((int)player_pos.x) + ", " + std::to_string((int)player_pos.y) + ")");
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in update_hud(): " << e.what() << std::endl;
    }
}