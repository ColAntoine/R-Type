/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Application Implementation - Client (inherits GameCore)
*/

#include "Application.hpp"
#include "Core/States/MainMenu/MainMenu.hpp"
#include "Core/States/Settings/Settings.hpp"
#include "Core/States/Lobby/Lobby.hpp"
#include "Core/States/WaitingLobby/WaitingLobby.hpp"
#include "Core/States/InGame/InGame.hpp"
#include "Entity/Components/Drawable/Drawable.hpp"
#include <iostream>

// ============================================================================
// Initialization
// ============================================================================

bool Application::initialize() {
    std::cout << "┌────────────────────────────────────────┐" << std::endl;
    std::cout << "│   R-Type Client Initialization         │" << std::endl;
    std::cout << "└────────────────────────────────────────┘" << std::endl;

    try {
        // ① Setup ECS base (from GameCore)
        std::cout << "⚙️  Setting up ECS base..." << std::endl;
        setup_ecs_base();

        // ② Load shared systems (from GameCore)
        std::cout << "⚙️  Loading shared systems..." << std::endl;
        load_shared_systems();

        // ③ Load client-specific systems (rendering, UI)
        std::cout << "⚙️  Loading client systems..." << std::endl;
        load_specific_systems();

        // ④ Setup services (client-only: Input, Network, Render)
        std::cout << "⚙️  Setting up services..." << std::endl;
        service_setup();

        // ⑤ Create local player entity
        // std::cout << "⚙️  Creating player entity..." << std::endl;
        // setup_player_entity();

        // ⑥ Create client-specific ECS systems
        std::cout << "⚙️  Initializing client systems..." << std::endl;
        input_system_ = std::make_unique<InputSystem>(&event_manager_, local_player_entity_);
        network_system_ = std::make_unique<NetworkSystem>(
            &event_manager_,
            &service_manager_.get_service<NetworkService>(),
            component_factory_
        );

        // ⑦ Subscribe to events
        std::cout << "⚙️  Setting up event handlers..." << std::endl;
        setup_event_handlers();

        // ⑧ Setup game states (MainMenu, Lobby, InGame, etc.)
        std::cout << "⚙️  Registering game states..." << std::endl;
        setup_game_states();

        running_ = true;
        initialized_ = true;

        std::cout << "✅ Client initialized successfully" << std::endl;
        std::cout << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "❌ Client initialization failed: " << e.what() << std::endl;
        return false;
    }
}

// ============================================================================
// Load Systems (override from GameCore)
// ============================================================================

void Application::load_specific_systems() {
    // Client-only systems (rendering, sprites, UI)
    system_loader_.load_system_from_so("lib/systems/libgame_Draw.so");
    system_loader_.load_system_from_so("lib/systems/libsprite_system.so");
    system_loader_.load_system_from_so("lib/systems/libgame_Control.so");

    std::cout << "✅ Client systems loaded" << std::endl;
}

// ============================================================================
// Service Setup (client-only)
// ============================================================================

void Application::service_setup() {
    // Register client services
    service_manager_.register_service<InputService>(&event_manager_);
    service_manager_.register_service<NetworkService>(&event_manager_);
    service_manager_.register_service<RenderService>(&event_manager_);

    // Initialize all services
    service_manager_.initialize();

    std::cout << "✅ Services initialized" << std::endl;
}

// ============================================================================
// Player Entity Setup
// ============================================================================

void Application::setup_player_entity() {
    // Create local player entity using factory
    local_player_entity_ = ecs_registry_.spawn_entity();

    // Add components using factory
    component_factory_->create_component<position>(
        ecs_registry_, local_player_entity_, 100.0f, 300.0f
    );

    component_factory_->create_component<velocity>(
        ecs_registry_, local_player_entity_, 0.0f, 0.0f
    );

    component_factory_->create_component<collider>(
        ecs_registry_, local_player_entity_, PLAYER_WIDTH, PLAYER_HEIGHT
    );

    component_factory_->create_component<drawable>(
        ecs_registry_, local_player_entity_,
        PLAYER_WIDTH, PLAYER_HEIGHT,
        255, 255, 255, 255  // White color
    );

    component_factory_->create_component<sprite>(
        ecs_registry_, local_player_entity_,
        "assets/REAPER_ICON.png",
        128.0f, 64.0f,  // Texture size
        1.0f, 1.0f      // Scale
    );

    std::cout << "✅ Player entity created (ID: " << local_player_entity_ << ")" << std::endl;
}

// ============================================================================
// Event Handlers
// ============================================================================

void Application::setup_event_handlers() {
    // Handle ESC key to quit
    event_manager_.subscribe<KeyPressEvent>([this](const KeyPressEvent& e) {
        if (e.key == KEY_ESCAPE) {
            std::cout << "ESC pressed, exiting..." << std::endl;
            running_ = false;
        }
    });

    // Handle network connection success
    event_manager_.subscribe<NetworkConnectedEvent>([this](const NetworkConnectedEvent& e) {
        local_player_id_ = e.player_id;
        std::cout << "✅ Connected to server (Player ID: " << e.player_id << ")" << std::endl;
    });

    // Handle network disconnection
    event_manager_.subscribe<NetworkDisconnectedEvent>([this][[maybe_unused]](const NetworkDisconnectedEvent& e) {
        std::cerr << "❌ Disconnected from server: " << std::endl;
        running_ = false;
    });

    std::cout << "✅ Event handlers registered" << std::endl;
}

// ============================================================================
// Game States Setup
// ============================================================================

void Application::setup_game_states() {
    // Register MainMenu (simple state)
    state_manager_.register_state<MainMenuState>("MainMenu");

    // Register Settings state with Application pointer
    state_manager_.register_state_with_factory("Settings", [this]() -> std::shared_ptr<IGameState> {
        return std::make_shared<SettingsState>(this);
    });

    // Register Lobby state
    state_manager_.register_state_with_factory("Lobby", [this]() -> std::shared_ptr<IGameState> {
        return std::make_shared<LobbyState>(this);
    });

    // Register Waiting Lobby state
    state_manager_.register_state_with_factory("WaitingLobby", [this]() -> std::shared_ptr<IGameState> {
        return std::make_shared<WaitingLobbyState>(this);
    });

    // Register InGame state
    state_manager_.register_state_with_factory("InGame", [this]() -> std::shared_ptr<IGameState> {
        return std::make_shared<InGameState>(this);
    });

    std::cout << "✅ Game states registered" << std::endl;
}

// ============================================================================
// Network Methods
// ============================================================================

bool Application::connect_to_server(const std::string& server_ip, int server_port) {
    std::cout << "Connecting to " << server_ip << ":" << server_port << "..." << std::endl;

    auto& network_service = service_manager_.get_service<NetworkService>();
    if (!network_service.connect_to_server(server_ip, server_port, player_name_)) {
        std::cerr << "❌ Failed to connect to server" << std::endl;
        return false;
    }

    std::cout << "✅ Connection initiated" << std::endl;
    return true;
}

void Application::send_ready_signal(bool ready) {
    auto& network_service = service_manager_.get_service<NetworkService>();
    network_service.send_ready_signal(ready);
    std::cout << "📤 Sent ready signal: " << (ready ? "READY" : "NOT READY") << std::endl;
}

// ============================================================================
// Main Loop
// ============================================================================

void Application::run() {
    if (!running_) {
        std::cerr << "❌ Application not initialized" << std::endl;
        return;
    }

    auto& render_service = service_manager_.get_service<RenderService>();

    // Start with main menu
    state_manager_.push_state("MainMenu");

    std::cout << "┌────────────────────────────────────────┐" << std::endl;
    std::cout << "│   🎮 Client Running                    │" << std::endl;
    std::cout << "└────────────────────────────────────────┘" << std::endl;
    std::cout << "Press ESC to quit" << std::endl;
    std::cout << std::endl;

    // Main game loop
    while (running_ && !render_service.should_close() && !state_manager_.is_empty()) {
        float delta_time = render_service.get_frame_time();

        // ① Emit frame update event
        event_manager_.emit(FrameUpdateEvent(delta_time));

        // ② Update services (Input, Network, Render)
        service_manager_.update(delta_time);

        // ③ Process events
        event_manager_.process_events();

        // ⑥ Update state manager (current game state)
        state_manager_.update(delta_time);

        // ⑦ Render frame
        render_service.begin_frame();

        // Let states handle their own rendering
        state_manager_.render();

        render_service.end_frame();

        // ⑧ Handle state input
        state_manager_.handle_input();
    }
    std::cout << std::endl;
    std::cout << "Client shutting down..." << std::endl;
    shutdown();
}

// ============================================================================
// ECS Systems Update
// ============================================================================

void Application::update_ecs_systems(float delta_time) {
    // Update event-driven ECS systems (Input, Network)
    input_system_->update(ecs_registry_, delta_time);
    network_system_->update(ecs_registry_, delta_time);
}

void Application::update_traditional_ecs_systems(float delta_time) {
    // Update all loaded systems through the DLLoader
    // (Position, Collision, LifeTime, Draw, Sprite, etc.)
    system_loader_.update_all_systems(ecs_registry_, delta_time);
}

// ============================================================================
// Shutdown
// ============================================================================

void Application::shutdown() {
    std::cout << "Shutting down services..." << std::endl;
    running_ = false;
    service_manager_.shutdown();
    std::cout << "✅ Client stopped cleanly" << std::endl;
}
