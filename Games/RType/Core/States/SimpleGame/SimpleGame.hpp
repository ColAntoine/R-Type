/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Simple Game State - Basic solo gameplay with player movement
*/

#pragma once

#include "Core/States/GameState.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Systems/UISystem.hpp"
#include "ECS/Components/UIComponent.hpp"
#include "ECS/UI/Components/Text.hpp"
#include <memory>
#include <vector>

// Forward declarations
class GameClient;

// Tag components for SimpleGame HUD elements
namespace RType {
    struct UIPlayerInfo : public IComponent {};
    struct UIInstructions : public IComponent {};
    struct UINetworkStatus : public IComponent {};
}

class SimpleGameState : public IGameState {
    private:
        GameClient* client_;
        bool initialized_{false};
        bool paused_{false};

        // ECS registries
        registry& game_registry_;  // Reference to client's game registry
        registry ui_registry_;     // UI registry

        // ECS UI system for HUD
        UI::UISystem ui_system_;

        // Player entity
        entity player_entity_{};

        // Game background state
        float bg_time_{0.0f};
        struct DataStream { float x; float y; float speed; float length; int chars; };
        std::vector<DataStream> bg_streams_;
        int bg_stream_count_{0};

        // Network placeholders
        bool network_connected_{false};
        float last_movement_confirmation_{0.0f};
        std::vector<std::pair<float, float>> predicted_positions_;

    public:
        SimpleGameState(GameClient* client, registry& game_registry);
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
        bool blocks_update() const override { return true; }
        bool blocks_render() const override { return true; }

    private:
        void setup_hud();
        void cleanup_hud();
        void update_hud();

        void create_player();
        void update_player(float delta_time);
        void handle_player_input();

        // Render helpers
        void render_falling_background();
        void render_game_entities();

        // Network placeholders
        void send_movement_to_server(float x, float y);
        void receive_server_confirmations();
        void apply_client_prediction();
};