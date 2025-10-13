/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Waiting Lobby State - Shows connected players and ready status
*/

#pragma once

#include "GameState.hpp"
#include "UI/UIManager.hpp"
#include "UI/Components/UIButton.hpp"
#include "UI/Components/UIText.hpp"
#include "UI/Components/UIPanel.hpp"
#include "Core/Events.hpp"
#include <vector>
#include <string>

// Forward declarations
class Application;

struct PlayerInfo {
    int player_id;
    std::string name;
    bool is_ready;
    bool is_local_player;
};

class WaitingLobbyState : public IGameState {
    private:
        Application* app_;
        UIManager ui_manager_;
        bool initialized_{false};
        std::vector<PlayerInfo> connected_players_;

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

        // Event handlers
        void handle_player_list_update(const PlayerListEvent& event);
        void handle_start_game(const StartGameEvent& event);
};