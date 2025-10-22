/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Simple Game State Implementation
*/

#include "SimpleGame.hpp"
#include "Core/Client/GameClient.hpp"
#include "Entity/Components/Controllable/Controllable.hpp"
#include "Entity/Components/Drawable/Drawable.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include <iostream>
#include <raylib.h>
#include <random>
#include <string>
#include <algorithm>

SimpleGameState::SimpleGameState(GameClient* client, registry& game_registry)
    : client_(client), game_registry_(game_registry) {}

void SimpleGameState::enter() {
    std::cout << "[SimpleGame] Entering state" << std::endl;

    if (!client_) {
        std::cerr << "ERROR: GameClient pointer is null in SimpleGameState::enter()" << std::endl;
        return;
    }

    // Register UI components
    ui_registry_.register_component<UI::UIComponent>();
    ui_registry_.register_component<RType::UIPlayerInfo>();
    ui_registry_.register_component<RType::UIInstructions>();
    ui_registry_.register_component<RType::UINetworkStatus>();

    setup_hud();
    create_player();

    // Initialize background streams
    bg_time_ = 0.0f;
    bg_streams_.clear();
    bg_stream_count_ = 20; // fewer streams for game
    std::mt19937 rng((unsigned)time(nullptr));
    std::uniform_real_distribution<float> xdist(0.0f, (float)GetScreenWidth());
    std::uniform_real_distribution<float> ydist(-600.0f, (float)GetScreenHeight());
    std::uniform_real_distribution<float> speeddist(50.0f, 150.0f);
    std::uniform_real_distribution<float> len(30.0f, 200.0f);
    std::uniform_int_distribution<int> chars(4, 20);
    for (int i = 0; i < bg_stream_count_; ++i) {
        DataStream s;
        s.x = xdist(rng);
        s.y = ydist(rng);
        s.speed = speeddist(rng);
        s.length = len(rng);
        s.chars = chars(rng);
        bg_streams_.push_back(s);
    }

    // Network placeholders
    network_connected_ = false; // Solo mode, no network
    predicted_positions_.clear();

    initialized_ = true;
    paused_ = false;
}

void SimpleGameState::exit() {
    std::cout << "[SimpleGame] Exiting state" << std::endl;
    cleanup_hud();

    // Remove player entity
    if (player_entity_ != entity{}) {
        game_registry_.kill_entity(player_entity_);
        player_entity_ = entity{};
    }

    initialized_ = false;
}

void SimpleGameState::pause() {
    std::cout << "[SimpleGame] Pausing state" << std::endl;
    paused_ = true;
    // Hide HUD when paused
    auto* ui_components = ui_registry_.get_if<UI::UIComponent>();
    if (ui_components) {
        for (auto& comp : *ui_components) {
            if (comp._ui_element) {
                comp._ui_element->setVisible(false);
            }
        }
    }
}

void SimpleGameState::resume() {
    std::cout << "[SimpleGame] Resuming state" << std::endl;
    paused_ = false;
    // Show HUD when resumed
    auto* ui_components = ui_registry_.get_if<UI::UIComponent>();
    if (ui_components) {
        for (auto& comp : *ui_components) {
            if (comp._ui_element) {
                comp._ui_element->setVisible(true);
            }
        }
    }
}

void SimpleGameState::update(float delta_time) {
    if (!initialized_ || paused_) return;

    // Update UI system
    ui_system_.update(ui_registry_, delta_time);
    update_hud();

    // Update player
    update_player(delta_time);

    // Update background
    bg_time_ += delta_time;
    // Use static RNG and distributions for consistency
    static std::mt19937 update_rng((unsigned)time(nullptr));
    static std::uniform_int_distribution<int> x_offset_dist(-50, 49);
    static std::uniform_int_distribution<int> speed_offset_dist(0, 99);
    for (auto &s : bg_streams_) {
        s.y += s.speed * delta_time;
        if (s.y - s.length > GetScreenHeight()) {
            s.y = -s.length * 0.2f;
            // Slightly randomize x and speed
            s.x += x_offset_dist(update_rng);
            if (s.x < 0) s.x += GetScreenWidth();
            if (s.x > GetScreenWidth()) s.x -= GetScreenWidth();
            s.speed = 60.0f + speed_offset_dist(update_rng);
        }
    }

    // Network placeholders
    receive_server_confirmations();
    apply_client_prediction();
}

void SimpleGameState::render() {
    if (!initialized_) return;

    // Draw the falling background
    render_falling_background();

    // Render game entities manually (since ECS systems may not be loaded)
    render_game_entities();

    // Render HUD on top
    ui_system_.render(ui_registry_);
}

void SimpleGameState::handle_input() {
    if (!initialized_ || paused_) return;

    // Handle pause menu (ESC key)
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (state_manager_) {
            // For now, just go back to solo lobby
            state_manager_->change_state("SoloLobby");
        }
    }

    // Handle player input
    handle_player_input();

    // Handle UI input
    ui_system_.process_input(ui_registry_);
}

void SimpleGameState::setup_hud() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();

    // Create player info text
    auto info_entity = ui_registry_.spawn_entity();
    auto info_text = std::shared_ptr<UI::UIText>(
        TextBuilder()
            .at(10, 10)
            .text("Player: Use WASD or Arrow Keys to move")
            .fontSize(20)
            .textColor({0, 229, 255, 255})
            .alignment(UI::TextAlignment::Left)
            .build(SCREEN_WIDTH, SCREEN_HEIGHT)
    );
    ui_registry_.add_component(info_entity, UI::UIComponent(info_text));
    ui_registry_.add_component(info_entity, RType::UIPlayerInfo{});

    // Create instructions text
    auto instr_entity = ui_registry_.spawn_entity();
    auto instr_text = std::shared_ptr<UI::UIText>(
        TextBuilder()
            .at(10, 35)
            .text("ESC: Back to Lobby")
            .fontSize(16)
            .textColor({200, 200, 200, 255})
            .alignment(UI::TextAlignment::Left)
            .build(SCREEN_WIDTH, SCREEN_HEIGHT)
    );
    ui_registry_.add_component(instr_entity, UI::UIComponent(instr_text));
    ui_registry_.add_component(instr_entity, RType::UIInstructions{});

    // Create network status text
    auto net_entity = ui_registry_.spawn_entity();
    auto net_text = std::shared_ptr<UI::UIText>(
        TextBuilder()
            .at(screen_width - 200, 10)
            .text("Network: Solo Mode (Offline)")
            .fontSize(16)
            .textColor({255, 255, 100, 255})
            .alignment(UI::TextAlignment::Left)
            .build(SCREEN_WIDTH, SCREEN_HEIGHT)
    );
    ui_registry_.add_component(net_entity, UI::UIComponent(net_text));
    ui_registry_.add_component(net_entity, RType::UINetworkStatus{});
}

void SimpleGameState::cleanup_hud() {
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
        ui_registry_.remove_component<RType::UIPlayerInfo>(ent);
        ui_registry_.remove_component<RType::UIInstructions>(ent);
        ui_registry_.remove_component<RType::UINetworkStatus>(ent);
    }
}

void SimpleGameState::update_hud() {
    // Update player position in HUD
    if (player_entity_ != entity{}) {
        auto* positions = game_registry_.get_if<position>();
        if (positions && positions->has(player_entity_)) {
            auto& pos = positions->get(player_entity_);
            auto* ui_components = ui_registry_.get_if<UI::UIComponent>();
            if (ui_components) {
                for (auto [comp, ent] : zipper(*ui_components)) {
                    if (comp._ui_element) {
                        auto text_elem = std::dynamic_pointer_cast<UI::UIText>(comp._ui_element);
                        if (text_elem) {
                            char buf[128];
                            std::snprintf(buf, sizeof(buf), "Player: (%.1f, %.1f) - Use WASD/Arrow Keys", pos.x, pos.y);
                            text_elem->setText(buf);
                            break; // Only update one
                        }
                    }
                }
            }
        }
    }
}

void SimpleGameState::create_player() {
    std::cout << "[SimpleGame] Creating player entity" << std::endl;

    player_entity_ = game_registry_.spawn_entity();

    // Add position component
    game_registry_.add_component(player_entity_, position{GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f});

    // Add velocity component
    game_registry_.add_component(player_entity_, velocity{0.0f, 0.0f});

    // Add drawable component (simple rectangle for now)
    game_registry_.add_component(player_entity_, drawable{32.0f, 32.0f, 0, 229, 255, 255});

    // Add controllable component
    game_registry_.add_component(player_entity_, controllable{200.0f});

    std::cout << "[SimpleGame] Player created" << std::endl;
}

void SimpleGameState::update_player(float delta_time) {
    if (player_entity_ == entity{}) return;

    // Get components
    auto* positions = game_registry_.get_if<position>();
    auto* velocities = game_registry_.get_if<velocity>();
    auto* controllables = game_registry_.get_if<controllable>();

    if (!positions || !velocities || !controllables) return;
    if (!positions->has(player_entity_) || !velocities->has(player_entity_) || !controllables->has(player_entity_)) return;

    auto& pos = positions->get(player_entity_);
    auto& vel = velocities->get(player_entity_);
    auto& ctrl = controllables->get(player_entity_);

    // Apply velocity to position
    pos.x += vel.vx * delta_time;
    pos.y += vel.vy * delta_time;

    // Keep player in bounds
    if (pos.x < 0) pos.x = 0;
    if (pos.x > GetScreenWidth() - 32) pos.x = GetScreenWidth() - 32;
    if (pos.y < 0) pos.y = 0;
    if (pos.y > GetScreenHeight() - 32) pos.y = GetScreenHeight() - 32;

    // Placeholder: Send movement to server (would be network call)
    send_movement_to_server(pos.x, pos.y);
}

void SimpleGameState::handle_player_input() {
    if (player_entity_ == entity{}) return;

    auto* velocities = game_registry_.get_if<velocity>();
    auto* controllables = game_registry_.get_if<controllable>();

    if (!velocities || !controllables) return;
    if (!velocities->has(player_entity_) || !controllables->has(player_entity_)) return;

    auto& vel = velocities->get(player_entity_);
    auto& ctrl = controllables->get(player_entity_);

    // Reset velocity
    vel.vx = 0.0f;
    vel.vy = 0.0f;

    // Handle input
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        vel.vy = -ctrl.speed;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        vel.vy = ctrl.speed;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        vel.vx = -ctrl.speed;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        vel.vx = ctrl.speed;
    }
}

void SimpleGameState::render_falling_background() {
    // Themed dynamic background
    ClearBackground({6, 8, 10, 255}); // very dark base

    // subtle neon grid overlay
    Color grid_color = {0, 40, 64, 20};
    int gw = 64;
    for (int x = 0; x < GetScreenWidth(); x += gw) {
        DrawLine(x, 0, x, GetScreenHeight(), grid_color);
    }
    for (int y = 0; y < GetScreenHeight(); y += gw) {
        DrawLine(0, y, GetScreenWidth(), y, grid_color);
    }

    // moving data streams (vertical)
    Color stream_col = {0, 229, 255, 120};
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
            DrawRectangle((int)s.x, (int)cy, 3, (int)step_h - 1, c);
        }
    }

    // moving horizontal scanline for energy feel
    int scan_y = (int)(std::fmod(bg_time_ * 60.0f, (float)(GetScreenHeight() + 200)) - 100);
    DrawRectangle(0, scan_y, GetScreenWidth(), 2, {0, 229, 255, 40});

    // vignette
    DrawRectangle(0, 0, GetScreenWidth(), 30, {0, 0, 0, 60});
    DrawRectangle(0, GetScreenHeight()-30, GetScreenWidth(), 30, {0, 0, 0, 60});
}

void SimpleGameState::render_game_entities() {
    // Render drawable entities
    auto* positions = game_registry_.get_if<position>();
    auto* drawables = game_registry_.get_if<drawable>();

    if (!positions || !drawables) return;

    for (auto [pos, drawable, ent] : zipper(*positions, *drawables)) {
        DrawRectangle(
            (int)pos.x,
            (int)pos.y,
            (int)drawable.w,
            (int)drawable.h,
            {drawable.r, drawable.g, drawable.b, drawable.a}
        );
    }
}

// Network placeholders
void SimpleGameState::send_movement_to_server(float x, float y) {
    // Placeholder: In real implementation, this would send position to server
    // For now, just store predicted positions
    predicted_positions_.push_back({x, y});
    if (predicted_positions_.size() > 10) {
        predicted_positions_.erase(predicted_positions_.begin());
    }
    last_movement_confirmation_ = GetTime();
}

void SimpleGameState::receive_server_confirmations() {
    // Placeholder: In real implementation, this would receive confirmations from server
    // For solo mode, just simulate occasional confirmations
    if (GetTime() - last_movement_confirmation_ > 0.1f) {
        // Simulate server confirmation
        last_movement_confirmation_ = GetTime();
    }
}

void SimpleGameState::apply_client_prediction() {
    // Placeholder: Apply client-side prediction corrections
    // In solo mode, no corrections needed
}