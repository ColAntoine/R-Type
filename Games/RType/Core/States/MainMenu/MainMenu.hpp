/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Main Menu Game State
*/

#pragma once

#include "Core/States/GameState.hpp"
#include <memory>
#include <vector>
#include <string>
#include <random>

#include "ECS/Registry.hpp"
#include "ECS/Systems/UISystem.hpp"
#include "ECS/Components/UIComponent.hpp"
#include "ECS/UI/Components/Button.hpp"
#include "ECS/UI/Components/Panel.hpp"
#include "ECS/UI/Components/Text.hpp"

// Tag components for identifying UI elements
// This allow us to get the UI elements individualy
namespace RType {
    struct UIMainPanel : public IComponent {};
    struct UITitleText : public IComponent {};
    struct UIPlayButton : public IComponent {};
    struct UISettingsButton : public IComponent {};
    struct UIQuitButton : public IComponent {};
}

class MainMenuState : public IGameState {
public:
    MainMenuState() = default;
    ~MainMenuState() override = default;

    // IGameState implementation
    void enter() override;
    void exit() override;
    void pause() override;
    void resume() override;

    void update(float delta_time) override;
    void render() override;
    void handle_input() override;

    std::string get_name() const override { return "MainMenu"; }

private:
    void setup_ui();
    void cleanup_ui();
    void update_reveal_animation(float delta_time);

    // Button callbacks
    void on_play_clicked();
    void on_settings_clicked();
    void on_quit_clicked();

    // ECS UI system
    registry ui_registry_;
    UI::UISystem ui_system_;

    bool initialized_{false};

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
    float menu_show_delay_{2.0f};
    float menu_elapsed_{0.0f};
    float menu_reveal_duration_{0.9f};
    float menu_reveal_progress_{0.0f};
};