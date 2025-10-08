/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Game State Interface
*/

#pragma once

#include <string>

// Forward declarations
class GameStateManager;

// Base interface for all game states
class IGameState {
public:
    virtual ~IGameState() = default;

    // State lifecycle methods
    virtual void enter() = 0;           // Called when entering this state
    virtual void exit() = 0;            // Called when leaving this state
    virtual void pause() = 0;           // Called when another state is pushed on top
    virtual void resume() = 0;          // Called when returning to this state

    // Core update methods
    virtual void update(float delta_time) = 0;
    virtual void render() = 0;
    virtual void handle_input() = 0;

    // State identification
    virtual std::string get_name() const = 0;
    virtual bool blocks_update() const { return true; }    // Does this state block states below it from updating?
    virtual bool blocks_render() const { return true; }    // Does this state block states below it from rendering?

protected:
    GameStateManager* state_manager_{nullptr};

    friend class GameStateManager;
    void set_state_manager(GameStateManager* manager) { state_manager_ = manager; }
};

// Game State Types
enum class GameStateType {
    MainMenu,
    Settings,
    Lobby,
    InGame,
    GameOver,
    Loading,
    WaitingLobby
};

// Helper function to convert state type to string
inline std::string state_type_to_string(GameStateType type) {
    switch (type) {
        case GameStateType::MainMenu: return "MainMenu";
        case GameStateType::Settings: return "Settings";
        case GameStateType::Lobby: return "Lobby";
        case GameStateType::InGame: return "InGame";
        case GameStateType::GameOver: return "GameOver";
        case GameStateType::Loading: return "Loading";
        case GameStateType::WaitingLobby: return "WaitingLobby";
        default: return "Unknown";
    }
}