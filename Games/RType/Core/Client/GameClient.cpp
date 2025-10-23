#include "GameClient.hpp"
#include <iostream>
#include "Network/UDPClient.hpp"
#include "Network/ClientService.hpp"
#include "Core/States/Loading/Loading.hpp"
#include "Core/States/MainMenu/MainMenu.hpp"
#include "Core/States/Lobby/Lobby.hpp"
#include "Core/States/SoloLobby/SoloLobby.hpp"
#include "Core/States/InGame/InGame.hpp"
#include "Network/NetworkManager.hpp"


GameClient::GameClient() {}
GameClient::~GameClient() {}

void GameClient::register_states() {
    std::cout << "[GameClient] Registering game states..." << std::endl;

    // Register all available states
    state_manager_.register_state<Loading>("Loading");
    state_manager_.register_state<MainMenuState>("MainMenu");
    // Register InGame with factory to inject the application's ECS registry reference and loader pointer
    state_manager_.register_state_with_factory("InGame", [this]() -> std::shared_ptr<IGameState> {
        return std::make_shared<InGameState>(this->ecs_registry_, &this->ecs_loader_);
    });
    state_manager_.register_state<LobbyState>("Lobby");
    state_manager_.register_state<SoloLobbyState>("SoloLobby");
}

bool GameClient::init()
{
    std::cout << "GameClient::init" << std::endl;
    AGameCore::RegisterComponents(ecs_registry_);
    load_systems();
    // Initialize Raylib window
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(1920, 1080, "R-Type - Solo Mode Available!");
    SetTargetFPS(60);

    if (!IsWindowReady()) {
        std::cerr << "[GameClient] Failed to initialize Raylib window" << std::endl;
        return false;
    }

    std::cout << "[GameClient] Raylib window initialized: 1024x768" << std::endl;

    // Register states
    register_states();

    // Start with loading screen
    state_manager_.push_state("Loading");

    // Create shared client service for in-game/network states
    auto client = std::make_shared<UdpClient>();
    RType::Network::set_client(client);

    // Create and start the network manager which encapsulates receive loop and dispatching
    network_manager_ = std::make_unique<NetworkManager>(client, ecs_registry_, ecs_loader_);
    network_manager_->register_default_handlers();
    network_manager_->start();

    running_ = true;
    std::cout << "[GameClient] Initialized successfully (No server required for Solo mode)" << std::endl;

    return true;
}

void GameClient::run()
{
    std::cout << "GameClient::run" << std::endl;

    float last_frame_time = 0.0f;

    while (running_ && !WindowShouldClose() && !state_manager_.is_empty()) {
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
        state_manager_.update(delta_time);
        state_manager_.render();
        render_mgr.end_frame();

        // Handle input
        state_manager_.handle_input();
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
    state_manager_.clear_states();

    // Notify server of voluntary disconnect if a network client exists
    if (auto client = RType::Network::get_client()) {
        uint32_t token = client->get_session_token();
        if (token != 0) {
            client->send_disconnect(token);
        }
    }
    // Close Raylib window
    if (IsWindowReady()) {
        CloseWindow();
    }

    running_ = false;
}

registry &GameClient::GetRegistry() { return ecs_registry_; }
DLLoader &GameClient::GetDLLoader() { return ecs_loader_; }

void GameClient::load_systems() {
    if (!ecs_loader_.load_components_from_so("lib/libECS.so", ecs_registry_)) {
        std::cerr << "[GameClient] Failed to load ECS components from shared library" << std::endl;
        return;
    }
    // Example system loading
    ecs_loader_.load_system_from_so("lib/systems/libanimation_system.so");
    ecs_loader_.load_system_from_so("lib/systems/libcollision_system.so");
    ecs_loader_.load_system_from_so("lib/systems/libgame_Control.so");
    ecs_loader_.load_system_from_so("lib/systems/libgame_Draw.so");
    ecs_loader_.load_system_from_so("lib/systems/libsprite_system.so");
    ecs_loader_.load_system_from_so("lib/systems/libposition_system.so");

    // Add more systems as needed
}
