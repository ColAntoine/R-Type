/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Simple Solo Game State
*/

#pragma once

#include "Core/States/GameState.hpp"
#include "Core/States/NetworkState.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components/IComponent.hpp"
#include <memory>
#include <vector>

// Tag components for identifying game entities
struct SimpleGamePlayer : public IComponent {
    float speed{300.f};
    float size{40.f};

    SimpleGamePlayer() = default;
    SimpleGamePlayer(float s, float sz) : speed(s), size(sz) {}
};

class GameClient;  // Forward declaration

class SimpleGameState : public IGameState {
public:
    SimpleGameState(GameClient* client, registry& reg, std::shared_ptr<NetworkState> network_state);
    ~SimpleGameState() override = default;

    // IGameState implementation
    void enter() override;
    void exit() override;
    void pause() override;
    void resume() override;

    void update(float delta_time) override;
    void render() override;
    void handle_input() override;

    std::string get_name() const override { return "SimpleGame"; }

private:
    GameClient* client_;
    registry& game_registry_;
    std::shared_ptr<NetworkState> network_state_;
    entity player_entity_{UINT32_MAX};
    bool initialized_{false};

    void setup_game();
    void cleanup_game();
    void spawn_player();
    void send_position_update(float x, float y);
    void render_falling_background();
    void update_background(float delta_time);
    void process_server_messages();

    // Client-side prediction placeholders
    void predict_local_movement();
    void apply_client_side_prediction();
    void perform_server_reconciliation();
    void rewind_and_replay_on_misprediction();
    void interpolate_remote_entity_positions();
    void smooth_remote_entity_movement();

    // Prediction state
    struct PredictedState {
        float x, y;
        uint32_t timestamp;
    };
    std::vector<PredictedState> prediction_history_;
    uint32_t last_server_timestamp_{0};
    PredictedState server_position_;

    // Game state
    bool game_running_{true};

    // Background animation state
    float bg_time_{0.0f};
    struct DataStream { float x; float y; float speed; float length; int chars; };
    std::vector<DataStream> bg_streams_;
    int bg_stream_count_{0};
};
