/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Settings Game State
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

class Application;

// Tag components for Settings UI elements
namespace RType {
    struct UISettingsPanel : public IComponent {};
    struct UISettingsTitle : public IComponent {};
    struct UINameLabel : public IComponent {};
    struct UINameInput : public IComponent {};
    struct UIAudioLabel : public IComponent {};
    struct UIGraphicsLabel : public IComponent {};
    struct UIBackButton : public IComponent {};
}

class SettingsState : public IGameState {
private:
    Application* app_;
    bool initialized_{false};

    // ECS UI system
    registry _uiRegistry;
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
    SettingsState(Application* app);
    ~SettingsState() override = default;

    // IGameState implementation
    void enter() override;
    void exit() override;
    void pause() override;
    void resume() override;

    void update(float delta_time) override;
    void render() override;
    void handle_input() override;

    std::string get_name() const override { return "Settings"; }
    bool blocks_render() const override { return false; } // Allow main menu to render behind

private:
    void setup_ui();
    void cleanup_ui();

    // Button callbacks
    void on_back_clicked();
    void on_player_name_changed(const std::string& name);
};