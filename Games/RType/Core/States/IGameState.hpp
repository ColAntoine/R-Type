/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Game State Interface
*/

#pragma once

#include <string>
#include <memory>
#include <ECS/Registry.hpp>
#include "ECS/Messaging/MessagingManager.hpp"
#include <iostream>

namespace EventTypes {
    const std::string SCREEN_PARAMETERS_CHANGED = "SCREEN_PARAMETERS_CHANGED";
}

// Forward declarations
class GameStateManager;

#ifdef _WIN32
    const std::string ext = ".dll";
#else
    const std::string ext = ".so";
#endif

// Base interface for all game states
class IGameState {
public:
    enum class GameStateType {
        MenusBackground,
        MainMenu,
        InGame,
        Settings,
        InGameHud,
        InGameBackground,
        InGamePause,
        Connection,
        SettingsPanel,
        Credits,
        Lobby,
        AudioSettings,
        VideoSettings,
        BindingsSettings,
        LoadingVideo,
        InGameExit
    };

    virtual ~IGameState() = default;

    // State lifecycle methods
    virtual void enter() = 0;           // Called when entering this state
    virtual void exit() = 0;            // Called when leaving this state
    virtual void pause() = 0;           // Called when another state is pushed on top
    virtual void resume() = 0;          // Called when returning to this state

    virtual void cleanup_ui() = 0;
    virtual void setup_ui() = 0;

    // Core update methods
    virtual void update(float delta_time) = 0;
    virtual void render() = 0;
    virtual void handle_input() = 0;

    // State identification
    virtual std::string get_name() const = 0;
    virtual bool blocks_update() const = 0;    // Does this state block states below it from updating?
    virtual bool blocks_render() const = 0;    // Does this state block states below it from rendering?
protected:
    virtual void set_state_manager(GameStateManager* manager) = 0;

    friend class GameStateManager;
};
