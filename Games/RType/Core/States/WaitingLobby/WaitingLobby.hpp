/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Waiting Lobby State - Shows connected players and ready status
*/

#pragma once

#include "GameState.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Systems/UISystem.hpp"
#include "ECS/Components/UIComponent.hpp"
#include "ECS/UI/Components/Panel.hpp"
#include "ECS/UI/Components/Text.hpp"
#include <vector>
#include <string>
#include <random>

// Forward declarations
class Application;

struct PlayerInfo {
    int player_id;
    std::string name;
    bool is_ready;
    bool is_local_player;
};

// Tag components for WaitingLobby UI elements
namespace RType {
    struct UIWaitingPanel : public IComponent {};
    struct UIWaitingTitle : public IComponent {};
    struct UIWaitingStatus : public IComponent {};
    struct UIPlayerListTitle : public IComponent {};
    struct UIPlayerSlot : public IComponent { int slot_index; };
    struct UIReadyButton : public IComponent {};
    struct UIDisconnectButton : public IComponent {};
    struct UIGameStatus : public IComponent {};
}

class WaitingLobbyState : public IGameState {
    private:
        Application* app_;
        bool initialized_{false};
        std::vector<PlayerInfo> connected_players_;

        // ECS UI system
        registry ui_registry_;
        UI::UISystem ui_system_;

        // ASCII background state
        std::vector<std::string> ascii_grid_;
        int ascii_cols_{0};
        int ascii_rows_{0};
        int ascii_font_size_{12};
        float ascii_timer_{0.0f};
        float ascii_interval_{0.04f};
        std::mt19937 ascii_rng_{std::random_device{}()};
        std::string ascii_charset_{" .,:;i!lI|/\\()1{}[]?-_+~<>^*abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};

    public:
        WaitingLobbyState(Application* app);
        ~WaitingLobbyState() override = default;

        // IGameState implementation
        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void update(float delta_time) override;
        void render() override;
        void handle_input() override;

        std::string get_name() const override { return "WaitingLobby"; }

    private:
        void setup_ui();
        void cleanup_ui();
        void update_player_list();
        void update_ready_status();
        void update_ready_button();

        // Button callbacks
        void on_ready_clicked();
        void on_back_clicked();

};