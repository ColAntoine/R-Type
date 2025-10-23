/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** In-Game State Implementation
*/

#include "InGame.hpp"
#include "Core/Client/Network/ClientService.hpp"
#include "Core/States/GameStateManager.hpp"
#include "Entity/Components/Score/Score.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "Entity/Components/Controllable/Controllable.hpp"
#include "ECS/Components.hpp"
#include <iostream>
#include <raylib.h>
#include <random>
#include <string>
#include <algorithm>

InGameState::InGameState(registry &ecs, DLLoader* loader)
: ecs_registry_(ecs), ecs_loader_(loader)
{}

void InGameState::enter() {
    std::cout << "[InGame] Entering state" << std::endl;

    // Register UI components
    ui_registry_.register_component<UI::UIComponent>();
    ui_registry_.register_component<RType::UIFPSText>();
    ui_registry_.register_component<RType::UIPlayerInfo>();
    ui_registry_.register_component<RType::UIConnectionStatus>();
    ui_registry_.register_component<RType::UIPositionText>();
    ui_registry_.register_component<RType::UIScoreText>();

    setup_hud();

    factory_ = ecs_loader_->get_factory();

    // entity test_player = ecs_registry_.spawn_entity();
    // factory_->create_component<position>(ecs_registry_, test_player, 300.0f, 300.0f);
    // factory_->create_component<velocity>(ecs_registry_, test_player, 0.0f, 0.0f);
    // factory_->create_component<controllable>(ecs_registry_, test_player, 100.0f);
    // factory_->create_component<animation>(ecs_registry_, test_player, "Games/RType/Assets/dedsec_eyeball-Sheet.png", 400.0f, 400.0f, 0.25f, 0.25f, 0, true);


    // Initialize background streams
    bg_time_ = 0.0f;
    bg_streams_.clear();
    bg_stream_count_ = 28; // number of vertical streams
    std::mt19937 rng((unsigned)time(nullptr));
    std::uniform_real_distribution<float> xdist(0.0f, (float)GetScreenWidth());
    std::uniform_real_distribution<float> ydist(-600.0f, (float)GetScreenHeight());
    std::uniform_real_distribution<float> speeddist(30.0f, 220.0f);
    std::uniform_real_distribution<float> len(40.0f, 400.0f);
    std::uniform_int_distribution<int> chars(6, 30);
    for (int i = 0; i < bg_stream_count_; ++i) {
        DataStream s;
        s.x = xdist(rng);
        s.y = ydist(rng);
        s.speed = speeddist(rng);
        s.length = len(rng);
        s.chars = chars(rng);
        bg_streams_.push_back(s);
    }

    initialized_ = true;
    paused_ = false;
}

void InGameState::exit() {
    std::cout << "[InGame] Exiting state" << std::endl;
    cleanup_hud();
    initialized_ = false;
}

void InGameState::pause() {
    std::cout << "[InGame] Pausing state" << std::endl;
    paused_ = true;
    // Hide HUD when paused
    auto* ui_components = ui_registry_.get_if<UI::UIComponent>();
    if (ui_components) {
        for (auto& comp : *ui_components) {
            if (comp._ui_element) {
                comp._ui_element->set_visible(false);
            }
        }
    }
}

void InGameState::resume() {
    std::cout << "[InGame] Resuming state" << std::endl;
    paused_ = false;
    // Show HUD when resumed
    auto* ui_components = ui_registry_.get_if<UI::UIComponent>();
    if (ui_components) {
        for (auto& comp : *ui_components) {
            if (comp._ui_element) {
                comp._ui_element->set_visible(true);
            }
        }
    }
}

void InGameState::update(float delta_time) {
    if (!initialized_ || paused_) return;

    // Update UI system
    ui_system_.update(ui_registry_, delta_time);
    update_hud();
    ecs_loader_->update_system_by_name("PositionSystem", ecs_registry_, delta_time);
    ecs_loader_->update_system_by_name("ControlSystem", ecs_registry_, delta_time);
    ecs_loader_->update_system_by_name("AnimationSystem", ecs_registry_, delta_time);

    // Update background
    bg_time_ += delta_time;
    // Use static RNG and distributions for consistency
    static std::mt19937 update_rng((unsigned)time(nullptr));
    static std::uniform_int_distribution<int> x_offset_dist(-100, 99);
    static std::uniform_int_distribution<int> speed_offset_dist(0, 159);
    for (auto &s : bg_streams_) {
        s.y += s.speed * delta_time;
        if (s.y - s.length > GetScreenHeight()) {
            s.y = -s.length * 0.2f;
            // Slightly randomize x and speed
            s.x += x_offset_dist(update_rng);
            if (s.x < 0) s.x += GetScreenWidth();
            if (s.x > GetScreenWidth()) s.x -= GetScreenWidth();
            s.speed = 80.0f + speed_offset_dist(update_rng);
        }
    }
}

void InGameState::render() {
    // Draw the falling background
    render_falling_background();

    // Render HUD on top
    ui_system_.render(ui_registry_);
}

void InGameState::render_falling_background() {
    // Themed dynamic background
    ClearBackground({6, 8, 10, 255}); // very dark base

    // subtle neon grid overlay
    Color grid_color = {0, 40, 64, 30};
    int gw = 48;
    for (int x = 0; x < GetScreenWidth(); x += gw) {
        DrawLine(x, 0, x, GetScreenHeight(), grid_color);
    }
    for (int y = 0; y < GetScreenHeight(); y += gw) {
        DrawLine(0, y, GetScreenWidth(), y, grid_color);
    }

    // moving data streams (vertical)
    Color stream_col = {0, 229, 255, 160};
    for (auto &s : bg_streams_) {
        int steps = s.chars;
        float step_h = s.length / (float)steps;
        for (int i = 0; i < steps; ++i) {
            float cy = s.y - i * step_h;
            if (cy < -20 || cy > GetScreenHeight() + 20) continue;
            // fade tail
            float t = 1.0f - (float)i / (float)steps;
            unsigned char a = (unsigned char)std::clamp<int>((int)(stream_col.a * t), 8, stream_col.a);
            Color c = {stream_col.r, stream_col.g, stream_col.b, a};
            // draw small rectangles as stream particles
            DrawRectangle((int)s.x, (int)cy, 4, (int)step_h - 1, c);
        }
    }

    // moving horizontal scanline for energy feel
    int scan_y = (int)(std::fmod(bg_time_ * 80.0f, (float)(GetScreenHeight() + 200)) - 100);
    DrawRectangle(0, scan_y, GetScreenWidth(), 2, {0, 229, 255, 60});

    // vignette
    DrawRectangle(0, 0, GetScreenWidth(), 40, {0, 0, 0, 80});
    DrawRectangle(0, GetScreenHeight()-40, GetScreenWidth(), 40, {0, 0, 0, 80});
}

void InGameState::handle_input() {
    // Handle pause menu (ESC key)
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (state_manager_) {
            // Push pause menu state
            // state_manager_->push_state("Pause");
        }
    }

    // Handle UI input (HUD is mostly read-only, but this is here for consistency)
    ui_system_.process_input(ui_registry_);

    // Game input is handled by the InputSystem in the Application
}

void InGameState::setup_hud() {
    // FPS display
    auto fps_entity = ui_registry_.spawn_entity();
    auto fps_text = std::make_shared<UI::UIText>(10, 10, "FPS: 0");
    UI::TextStyle fps_style;
    fps_style._text_color = {255, 255, 0, 255}; // Yellow
    fps_style._font_size = 20;
    fps_style._alignment = UI::TextAlignment::Left;
    fps_text->set_style(fps_style);
    ui_registry_.add_component(fps_entity, UI::UIComponent(fps_text));
    ui_registry_.add_component(fps_entity, RType::UIFPSText{});

    // Player info
    auto player_entity = ui_registry_.spawn_entity();
    auto player_info = std::make_shared<UI::UIText>(10, 35, "Player: 0");
    UI::TextStyle player_style;
    player_style._text_color = {255, 255, 255, 255}; // White
    player_style._font_size = 20;
    player_style._alignment = UI::TextAlignment::Left;
    player_info->set_style(player_style);
    ui_registry_.add_component(player_entity, UI::UIComponent(player_info));
    ui_registry_.add_component(player_entity, RType::UIPlayerInfo{});

    // Connection status
    auto status_entity = ui_registry_.spawn_entity();
    auto connection_status = std::make_shared<UI::UIText>(10, 60, "Status: Unkown");
    UI::TextStyle status_style;
    status_style._text_color = {255, 100, 100, 255}; // Light red
    status_style._font_size = 20;
    status_style._alignment = UI::TextAlignment::Left;
    connection_status->set_style(status_style);
    ui_registry_.add_component(status_entity, UI::UIComponent(connection_status));
    ui_registry_.add_component(status_entity, RType::UIConnectionStatus{});

    // Position display
    auto pos_entity = ui_registry_.spawn_entity();
    auto position_text = std::make_shared<UI::UIText>(10, 85, "Position: (0, 0)");
    UI::TextStyle pos_style;
    pos_style._text_color = {200, 200, 200, 255}; // Light gray
    pos_style._font_size = 20;
    pos_style._alignment = UI::TextAlignment::Left;
    position_text->set_style(pos_style);
    ui_registry_.add_component(pos_entity, UI::UIComponent(position_text));
    ui_registry_.add_component(pos_entity, RType::UIPositionText{});

    // Score display
    auto score_entity = ui_registry_.spawn_entity();
    auto score_text = std::make_shared<UI::UIText>(10, 110, "Score: 0");
    UI::TextStyle score_style;
    score_style._text_color = {200, 200, 200, 255};
    score_style._font_size = 20;
    score_style._alignment = UI::TextAlignment::Left;
    score_text->set_style(score_style);
    ui_registry_.add_component(score_entity, UI::UIComponent(score_text));
    ui_registry_.add_component(score_entity, RType::UIScoreText{});

    std::cout << "[InGame] HUD setup complete" << std::endl;
}

void InGameState::cleanup_hud() {
    // Registry entities will be cleaned up automatically
}

void InGameState::update_hud() {
}