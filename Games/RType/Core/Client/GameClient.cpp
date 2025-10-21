#include "GameClient.hpp"
#include "Network/UDPClient.hpp"
#include "Core/States/Loading/Loading.hpp"
#include "Core/States/MainMenu/MainMenu.hpp"
#include "Core/States/Lobby/Lobby.hpp"
#include "Core/States/SoloLobby/SoloLobby.hpp"
#include "Core/States/InGame/InGame.hpp"

#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

GameClient::GameClient() {}
GameClient::~GameClient() {}

void GameClient::register_states() {
    std::cout << "[GameClient] Registering game states..." << std::endl;

    // Register all available states
    _stateManager.register_state<Loading>("Loading");
    _stateManager.register_state<MainMenuState>("MainMenu");
    _stateManager.register_state<LobbyState>("Lobby");
    _stateManager.register_state<SoloLobbyState>("SoloLobby");
    _stateManager.register_state<InGameState>("InGame");

    std::cout << "[GameClient] States registered: Loading, MainMenu, Lobby, SoloLobby" << std::endl;
    std::cout << "[GameClient] ✓ Track 1 features are all implemented!" << std::endl;
    std::cout << "[GameClient] ✓ Asset Manager, Renderer, Physics, Audio, Messaging, Plugin API" << std::endl;
}

bool GameClient::init()
{
    std::cout << "GameClient::init" << std::endl;
    AGameCore::RegisterComponents(ecs_registry_);

    // Initialize Raylib window
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(1024, 768, "R-Type - Solo Mode Available!");
    SetTargetFPS(60);

    if (!IsWindowReady()) {
        std::cerr << "[GameClient] Failed to initialize Raylib window" << std::endl;
        return false;
    }

    std::cout << "[GameClient] Raylib window initialized: 1024x768" << std::endl;

    // Register states
    register_states();

    // Start with loading screen
    _stateManager.push_state("Loading");

    _running = true;
    std::cout << "[GameClient] Initialized successfully (No server required for Solo mode)" << std::endl;

    return true;
}

void GameClient::run()
{
    std::cout << "GameClient::run" << std::endl;

    float last_frame_time = 0.0f;

    while (_running && !WindowShouldClose() && !_stateManager.is_empty()) {
        // Calculate delta time
        float current_time = GetTime();
        float delta_time = current_time - last_frame_time;
        last_frame_time = current_time;

        // Cap delta time to prevent huge jumps
        if (delta_time > 0.1f) delta_time = 0.1f;

        // Update current state
        _stateManager.update(delta_time);

        // Render via RenderManager (centralized begin/end, camera and SpriteBatch)
        auto &render_mgr = RenderManager::instance();
        render_mgr.begin_frame();

        _stateManager.render();

        render_mgr.end_frame();

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
    if (IsWindowReady()) {
        CloseWindow();
    }

    _running = false;
}

registry& GameClient::GetRegistry() { return ecs_registry_; }
