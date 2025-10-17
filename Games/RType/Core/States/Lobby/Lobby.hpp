/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Lobby Game State
*/

#pragma once

#include "Core/States/GameState.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Systems/UISystem.hpp"
#include "ECS/Components/UIComponent.hpp"
#include "ECS/UI/Components/Button.hpp"
#include "ECS/UI/Components/Panel.hpp"
#include "ECS/UI/Components/Text.hpp"
#include "ECS/UI/Components/InputField.hpp"
#include <memory>
#include <vector>
#include <string>
#include <random>

// Forward declarations
class Application;

// Tag components for Lobby UI elements
namespace RType {
    struct UILobbyPanel : public IComponent {};
    struct UILobbyTitle : public IComponent {};
    struct UIIPLabel : public IComponent {};
    struct UIIPInput : public IComponent {};
    struct UIPortLabel : public IComponent {};
    struct UIPortInput : public IComponent {};
    struct UIConnectButton : public IComponent {};
    struct UILobbyBackButton : public IComponent {};
}

class LobbyState : public IGameState {
private:
    Application* app_;
    bool initialized_{false};
    std::string server_ip_{"127.0.0.1"};
    int server_port_{8080};

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