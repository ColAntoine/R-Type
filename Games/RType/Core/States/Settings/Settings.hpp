/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Settings Game State
*/

#pragma once

#include "Core/States/GameState.hpp"
#include "UI/UIManager.hpp"
#include "UI/Components/UIButton.hpp"
#include "UI/Components/UIText.hpp"
#include "UI/Components/UIPanel.hpp"
#include "UI/Components/UIInputField.hpp"
#include <memory>

class Application;

class SettingsState : public IGameState {
private:
    Application* app_;
    UIManager ui_manager_;
    bool initialized_{false};

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