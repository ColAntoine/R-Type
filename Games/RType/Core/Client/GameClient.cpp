#include "GameClient.hpp"
#include "Network/UDPClient.hpp"
#include "ECS/Renderer/RenderManager.hpp"

#include "Core/States/MainMenu/MainMenu.hpp"
#include "Core/States/InGame/InGame.hpp"
#include "Core/States/InGameHud/InGameHud.hpp"
#include "Core/States/MenusBG/MenusBG.hpp"
#include "Core/States/Connection/Connection.hpp"
#include "Core/States/Settings/Settings.hpp"
#include "Core/States/SettingsPanel/SettingsPanel.hpp"
#include "Core/States/Credits/Credits.hpp"

#include "Constants.hpp"

#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

auto &renderManager = RenderManager::instance();

GameClient::GameClient() {}
GameClient::~GameClient() {}

void GameClient::register_states() {
    std::cout << "[GameClient] Registering game states..." << std::endl;

    // Register all available states
    _stateManager.register_state<MenusBackgroundState>("MenusBackground");
    _stateManager.register_state<MainMenuState>("MainMenu");
    _stateManager.register_state<InGameState>("InGame");
    _stateManager.register_state<InGameHudState>("InGameHud");
    _stateManager.register_state<SettingsState>("Settings");
    _stateManager.register_state<Connection>("Connection");
    _stateManager.register_state<SettingsPanelState>("SettingsPanel");
    _stateManager.register_state<CreditsState>("Credits");
}

bool GameClient::init()
{
    std::cout << "GameClient::init" << std::endl;

    renderManager.init("R-Type");

    if (!renderManager.is_window_ready()) {
        std::cerr << "[GameClient] Failed to initialize Raylib window" << std::endl;
        return false;
    }

    // Register states
    register_states();

    // Start with loading screen
    _stateManager.push_state("MenusBackground");
    _stateManager.push_state("MainMenu");

    _running = true;
    std::cout << "[GameClient] Initialized successfully (No server required for Solo mode)" << std::endl;

    return true;
}

void GameClient::run()
{
    std::cout << "GameClient::run" << std::endl;

    float last_frame_time = 0.0f;

    while (_running && !renderManager.window_should_close() && !_stateManager.is_empty()) {
        // Calculate delta time
        float current_time = GetTime();
        float delta_time = current_time - last_frame_time;
        last_frame_time = current_time;

        // Cap delta time to prevent huge jumps
        if (delta_time > 0.1f) delta_time = 0.1f;

        // Update current state
        _stateManager.update(delta_time);

        // Render via RenderManager (centralized begin/end, camera and SpriteBatch)
        renderManager.begin_frame();

        _stateManager.render();

        renderManager.end_frame();

        // Handle input
        _stateManager.handle_input();
    }
}

void GameClient::update(float delta)
{
    // State manager handles updates now
    (void)delta;
}

void GameClient::shutdown()
{
    std::cout << "GameClient::shutdown" << std::endl;

    // Clear all states
    _stateManager.clear_states();

    // Close Raylib window
    if (renderManager.is_window_ready()) {
        renderManager.shutdown();
    }

    _running = false;
}
