/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Lobby Game State
*/

#pragma once

#include "Core/States/GameState.hpp"
#include "UI/UIManager.hpp"
#include "UI/Components/UIButton.hpp"
#include "UI/Components/UIText.hpp"
#include "UI/Components/UIPanel.hpp"
#include "UI/Components/UIInputField.hpp"
#include <memory>

// Forward declarations
class Application;

class LobbyState : public IGameState {
private:
    Application* app_;
    UIManager ui_manager_;
    bool initialized_{false};
    std::string server_ip_{"127.0.0.1"};
    int server_port_{8080};

public:
    LobbyState(Application* app);
    ~LobbyState() override = default;

    // IGameState implementation
    void enter() override;
    void exit() override;
    void pause() override;
    void resume() override;

    void update(float delta_time) override;
    void render() override;
    void handle_input() override;

    std::string get_name() const override { return "Lobby"; }

private:
    void setup_ui();
    void cleanup_ui();

    // Button callbacks
    void on_connect_clicked();
    void on_back_clicked();
    void on_server_ip_changed(const std::string& ip);
    void on_server_port_changed(const std::string& port);
};