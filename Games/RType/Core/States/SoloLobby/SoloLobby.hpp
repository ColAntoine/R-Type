/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Solo Lobby Game State - Simple setup for solo play
*/

#pragma once

#include "Core/States/GameState.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Systems/UISystem.hpp"
#include "ECS/Components/UIComponent.hpp"
#include "ECS/UI/Components/Button.hpp"
#include "ECS/UI/Components/Panel.hpp"
#include "ECS/UI/Components/Text.hpp"
#include <memory>
#include <random>

// Tag components for Solo Lobby UI elements
namespace RType {
    struct UISoloLobbyPanel : public IComponent {};
    struct UISoloLobbyTitle : public IComponent {};
    struct UIStartSoloButton : public IComponent {};
    struct UISoloLobbyBackButton : public IComponent {};
}

class SoloLobbyState : public IGameState {
private:
    bool initialized_{false};

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

    // Menu appearance timing
    float menu_show_delay_{1.0f};
    float menu_elapsed_{0.0f};
    float menu_reveal_duration_{0.8f};
    float menu_reveal_progress_{0.0f};

public:
    SoloLobbyState() = default;
    ~SoloLobbyState() override = default;

    // IGameState implementation
    void enter() override;
    void exit() override;
    void pause() override;
    void resume() override;

    void update(float delta_time) override;
    void render() override;
    void handle_input() override;

    std::string get_name() const override { return "SoloLobby"; }

private:
    void setup_ui();
    void cleanup_ui();
    void update_reveal_animation(float delta_time);
    void render_ascii_background();

    // Button callbacks
    void on_start_solo_clicked();
    void on_back_clicked();
};