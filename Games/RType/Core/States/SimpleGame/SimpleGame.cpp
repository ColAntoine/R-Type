/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Simple Solo Game State Implementation
*/

#include "SimpleGame.hpp"
#include "Core/Client/GameClient.hpp"
#include "Core/States/GameStateManager.hpp"
#include "Core/Server/Protocol/Protocol.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/Zipper.hpp"
#include <iostream>
#include <raylib.h>
#include <cmath>
#include <random>
#include <algorithm>

SimpleGameState::SimpleGameState(GameClient* client, registry& reg, std::shared_ptr<NetworkState> network_state)
    : client_(client), game_registry_(reg), network_state_(network_state) {}

void SimpleGameState::enter() {
    std::cout << "[SimpleGame] Entering Solo Game State" << std::endl;

    // Register game-specific components
    game_registry_.register_component<SimpleGamePlayer>();
    game_registry_.register_component<position>();
    game_registry_.register_component<velocity>();

    setup_game();
    game_running_ = true;
    initialized_ = true;

    std::cout << "[SimpleGame] Solo Game initialized successfully" << std::endl;
}

void SimpleGameState::exit() {
    std::cout << "[SimpleGame] Exiting Solo Game State" << std::endl;
    cleanup_game();
    initialized_ = false;
}

void SimpleGameState::pause() {
    std::cout << "[SimpleGame] Pausing Solo Game" << std::endl;
}

void SimpleGameState::resume() {
    std::cout << "[SimpleGame] Resuming Solo Game" << std::endl;
}

void SimpleGameState::setup_game() {
    spawn_player();
    
    // Initialize background streams
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> x_dist(0.f, static_cast<float>(GetScreenWidth()));
    std::uniform_real_distribution<float> speed_dist(30.f, 80.f);
    
    bg_stream_count_ = 8;
    bg_streams_.clear();
    for (int i = 0; i < bg_stream_count_; ++i) {
        DataStream stream;
        stream.x = x_dist(gen);
        stream.y = -100.f;
        stream.speed = speed_dist(gen);
        stream.length = 150.f;
        stream.chars = 20;
        bg_streams_.push_back(stream);
    }
}

void SimpleGameState::cleanup_game() {
    // Clean up entities if needed
}

void SimpleGameState::spawn_player() {
    player_entity_ = game_registry_.spawn_entity();

    // Add player component - use emplace_component to construct in place
    game_registry_.emplace_component<SimpleGamePlayer>(player_entity_, 300.f, 40.f);

    // Add position component (center of screen)
    float start_x = GetScreenWidth() / 2.f - 20.f;
    float start_y = GetScreenHeight() / 2.f - 20.f;
    game_registry_.emplace_component<position>(player_entity_, start_x, start_y);

    // Add velocity component
    game_registry_.emplace_component<velocity>(player_entity_, 0.f, 0.f);

    std::cout << "[SimpleGame] Player spawned at (" << start_x << ", " << start_y << ")" << std::endl;
}

void SimpleGameState::send_position_update(float x, float y) {
    // Only send if connected to server
    if (!network_state_ || !network_state_->connected || !network_state_->udp_client) {
        return;
    }

    // Build POSITION_UPDATE packet
    RType::Protocol::PacketBuilder builder;
    builder.begin_packet(static_cast<uint8_t>(RType::Protocol::GameMessage::POSITION_UPDATE));
    
    RType::Protocol::PositionUpdate pos_update;
    pos_update.entity_id = network_state_->player_id;
    pos_update.x = x;
    pos_update.y = y;
    pos_update.vx = 0.f;
    pos_update.vy = 0.f;
    pos_update.timestamp = static_cast<uint32_t>(GetTime() * 1000);
    
    builder.add_struct(pos_update);
    auto packet = builder.finalize();
    
    // Send to server
    network_state_->udp_client->send(packet.data(), packet.size());
}

void SimpleGameState::update(float delta_time) {
    if (!initialized_ || !game_running_) return;

    // Update background animation
    update_background(delta_time);

    // Get player components
    auto* player_components = game_registry_.get_if<SimpleGamePlayer>();
    auto* position_components = game_registry_.get_if<position>();
    auto* velocity_components = game_registry_.get_if<velocity>();

    if (!player_components || !position_components || !velocity_components) {
        return;
    }

        // Update player based on input
    for (auto [player, pos, vel, ent] : zipper(*player_components, *position_components, *velocity_components)) {
        // Reset velocity
        vel.vx = 0.f;
        vel.vy = 0.f;

        // Handle input
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
            vel.vy = -player.speed;
        }
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
            vel.vy = player.speed;
        }
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            vel.vx = -player.speed;
        }
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            vel.vx = player.speed;
        }

        // Apply client-side prediction
        predict_local_movement();
        apply_client_side_prediction();

        // Update position
        pos.x += vel.vx * delta_time;
        pos.y += vel.vy * delta_time;

        // Clamp position to screen bounds
        if (pos.x < 0.f) pos.x = 0.f;
        if (pos.x + player.size > GetScreenWidth()) pos.x = GetScreenWidth() - player.size;
        if (pos.y < 0.f) pos.y = 0.f;
        if (pos.y + player.size > GetScreenHeight()) pos.y = GetScreenHeight() - player.size;

        // Send position update to server
        send_position_update(pos.x, pos.y);

        // Perform server reconciliation if needed
        perform_server_reconciliation();
        rewind_and_replay_on_misprediction();
    }
}

void SimpleGameState::render() {
    if (!initialized_) return;

    // Entity interpolation for smooth remote movement
    interpolate_remote_entity_positions();
    smooth_remote_entity_movement();

    // Draw animated background
    render_falling_background();

    // Draw player
    auto* player_components = game_registry_.get_if<SimpleGamePlayer>();
    auto* position_components = game_registry_.get_if<position>();

    if (player_components && position_components) {
        for (auto [player, pos, ent] : zipper(*player_components, *position_components)) {
            // Draw player square
            DrawRectangle(
                static_cast<int>(pos.x),
                static_cast<int>(pos.y),
                static_cast<int>(player.size),
                static_cast<int>(player.size),
                {0, 229, 255, 255}
            );
            
            // Draw border
            DrawRectangleLines(
                static_cast<int>(pos.x),
                static_cast<int>(pos.y),
                static_cast<int>(player.size),
                static_cast<int>(player.size),
                {255, 100, 200, 255}
            );
        }
    }

    // Draw UI text
    DrawText("SOLO MODE - Use WASD or Arrow Keys to Move", 10, 10, 20, {0, 229, 255, 255});
    DrawText("Press ESC to Return to Menu", 10, 40, 20, {200, 200, 200, 255});
}

void SimpleGameState::handle_input() {
    if (!initialized_) return;

    // ESC to return to menu
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (state_manager_) {
            state_manager_->change_state("SoloLobby");
        }
    }

    // Placeholder for server-related input handling
    // TODO: Send player inputs to server when networked mode is enabled
    // TODO: Receive server confirmation and other player inputs
}

void SimpleGameState::update_background(float delta_time) {
    bg_time_ += delta_time;

    // Update background streams
    for (auto& stream : bg_streams_) {
        stream.y += stream.speed * delta_time;
        
        // Reset stream when it goes off screen
        if (stream.y > GetScreenHeight() + 100.f) {
            stream.y = -100.f;
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> x_dist(0.f, static_cast<float>(GetScreenWidth()));
            stream.x = x_dist(gen);
        }
    }
}

void SimpleGameState::render_falling_background() {
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

// Client-side prediction placeholders
void SimpleGameState::predict_local_movement() {
    // Placeholder for predicting local movement
    // Predict own movement locally for responsive feel
}

void SimpleGameState::apply_client_side_prediction() {
    // Placeholder for applying client-side prediction
    // Apply predicted movement before server confirmation
}

void SimpleGameState::perform_server_reconciliation() {
    // Placeholder for server reconciliation
    // Check for mispredictions and correct client state
}

void SimpleGameState::rewind_and_replay_on_misprediction() {
    // Placeholder for rewinding and replaying on misprediction
    // Rewind to server state and replay inputs
}

void SimpleGameState::interpolate_remote_entity_positions() {
    // Placeholder for interpolating remote entity positions
    // Smooth movement of remote entities between server updates
}

void SimpleGameState::smooth_remote_entity_movement() {
    // Placeholder for smoothing remote entity movement
    // Apply interpolation to make remote entities move smoothly
}

