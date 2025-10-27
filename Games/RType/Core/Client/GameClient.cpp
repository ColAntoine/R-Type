#include "GameClient.hpp"
#include "Network/UDPClient.hpp"
#include "Core/Client/Network/ClientService.hpp"
#include "Core/Client/Network/NetworkService.hpp"
#include "ECS/Renderer/RenderManager.hpp"

#include "Core/States/MainMenu/MainMenu.hpp"
#include "Core/States/InGame/InGame.hpp"
#include "Core/States/InGameHud/InGameHud.hpp"
#include "Core/States/MenusBG/MenusBG.hpp"
#include "Core/States/Connection/Connection.hpp"
#include "Core/States/Settings/Settings.hpp"
#include "Core/States/SettingsPanel/SettingsPanel.hpp"
#include "Core/States/Credits/Credits.hpp"
#include "Core/States/Lobby/Lobby.hpp"
#include "Core/States/AudioSettings/AudioSettings.hpp"
#include "Core/States/VideoSettings/VideoSettings.hpp"
#include "Core/States/BindsSettings/BindsSettings.hpp"

#include "Constants.hpp"

#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <string>

auto &renderManager = RenderManager::instance();

GameClient::GameClient() {}
GameClient::~GameClient() {}

void GameClient::register_states() {
    std::cout << "[GameClient] Registering game states..." << std::endl;

    // Register all available states
    _stateManager.register_state<MenusBackgroundState>("MenusBackground");
    _stateManager.register_state<MainMenuState>("MainMenu");

    // Register InGame with shared registry for multiplayer
    _stateManager.register_state_with_factory("InGame", [this]() -> std::shared_ptr<IGameState> {
        return std::make_shared<InGameState>(&this->ecs_registry_, &this->ecs_loader_);
    });

    _stateManager.register_state<InGameHudState>("InGameHud");
    _stateManager.register_state<SettingsState>("Settings");
    _stateManager.register_state<Connection>("Connection");
    _stateManager.register_state<SettingsPanelState>("SettingsPanel");
    _stateManager.register_state<CreditsState>("Credits");
    _stateManager.register_state<Lobby>("Lobby");
    _stateManager.register_state<AudioSettingsState>("AudioSettings");
    _stateManager.register_state<VideoSettingsState>("VideoSettings");
    _stateManager.register_state<BindsSettingsState>("BindsSettings");
}

bool GameClient::init()
{
    std::cout << "GameClient::init" << std::endl;

    renderManager.init("R-Type");

    std::string fontPath = std::string(RTYPE_PATH_ASSETS) + "HACKED.ttf";
    if (!renderManager.load_font(fontPath.c_str())) {
        std::cerr << "[GameClient] Failed to load font: " << fontPath << std::endl;
        std::cerr << "Using default font." << std::endl;
    }

    if (!renderManager.is_window_ready()) {
        std::cerr << "[GameClient] Failed to initialize Raylib window" << std::endl;
        return false;
    }

    // Register states
    register_states();

    // Start with loading screen
    _stateManager.push_state("MenusBackground");
    _stateManager.push_state("MainMenu");

    // Load components into shared registry BEFORE starting network manager
    // This ensures components are registered when PLAYER_SPAWN messages arrive
    std::cout << "[GameClient] Loading components into shared registry..." << std::endl;
    ecs_loader_.load_components_from_so("build/lib/libECS.so", ecs_registry_);

    // Create shared client service for in-game/network states
    auto client = std::make_shared<UdpClient>();
    RType::Network::set_client(client);

    // Create and start the network manager which encapsulates receive loop and dispatching
    network_manager_ = std::make_unique<NetworkManager>(client, ecs_registry_, ecs_loader_);
    network_manager_->register_default_handlers();
    network_manager_->start();

    // Set network manager in service for states to access
    RType::Network::set_network_manager(network_manager_.get());

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

        // Render via RenderManager (centralized begin/end, camera and SpriteBatch)
        auto &render_mgr = RenderManager::instance();
        render_mgr.begin_frame();
        if (network_manager_) network_manager_->process_pending();
        _stateManager.update(delta_time);
        _stateManager.render();
        render_mgr.end_frame();

        // Handle input
        _stateManager.handle_input();
    }
}

void GameClient::update(float delta)
{
    (void)delta;
}

void GameClient::shutdown()
{
    std::cout << "GameClient::shutdown" << std::endl;

    // Clear all states
    _stateManager.clear_states();

    // Notify server of voluntary disconnect if a network client exists
    if (auto client = RType::Network::get_client()) {
        uint32_t token = client->get_session_token();
        if (token != 0) {
            client->send_disconnect(token);
        }
    }
    // Close Raylib window
    if (renderManager.is_window_ready()) {
        renderManager.shutdown();
    }

    _running = false;
}

registry &GameClient::GetRegistry() { return ecs_registry_; }
DLLoader &GameClient::GetDLLoader() { return ecs_loader_; }