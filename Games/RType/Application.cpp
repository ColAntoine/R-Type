#include "Application.hpp"
#include "States/MainMenu/MainMenu.hpp"
#include "States/Settings/Settings.hpp"
#include "States/Lobby/Lobby.hpp"
#include "States/WaitingLobby/WaitingLobby.hpp"
#include "States/InGame/InGame.hpp"
#include "Entity/Components/Drawable/Drawable.hpp"


void Application::load_systems() {
    system_loader_.load_system_from_so("lib/systems/libposition_system.so");
    system_loader_.load_system_from_so("lib/systems/libcollision_system.so");
    system_loader_.load_system_from_so("lib/systems/libgame_LifeTime.so");
    system_loader_.load_system_from_so("lib/systems/libgame_Draw.so");
    system_loader_.load_system_from_so("lib/systems/libsprite_system.so");
}

void Application::service_setup() {
    // Setup services
    service_manager_.register_service<InputService>(&event_manager_);
    service_manager_.register_service<NetworkService>(&event_manager_);
    service_manager_.register_service<RenderService>(&event_manager_);
    service_manager_.initialize();
}

bool Application::initialize() {
    try {
        setup_ecs();
        load_systems();
        service_setup();

        // Create ECS systems
        input_system_ = std::make_unique<InputSystem>(&event_manager_, local_player_entity_);
        network_system_ = std::make_unique<NetworkSystem>(&event_manager_,
            &service_manager_.get_service<NetworkService>(), component_factory_);

        // Subscribe to application events
        setup_event_handlers();

        // Setup game states
        setup_game_states();

        running_ = true;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Application initialization failed: " << e.what() << std::endl;
        return false;
    }
}

bool Application::connect_to_server(const std::string& server_ip, int server_port) {
    auto& network_service = service_manager_.get_service<NetworkService>();
    if (!network_service.connect_to_server(server_ip, server_port, player_name_)) {
        return false;
    }
    return true;
}

void Application::send_ready_signal(bool ready) {
    auto& network_service = service_manager_.get_service<NetworkService>();
    network_service.send_ready_signal(ready);
}

void Application::run() {
    if (!running_) {
        std::cerr << "Application not initialized" << std::endl;
        return;
    }

    auto& render_service = service_manager_.get_service<RenderService>();

    // Start with main menu
    state_manager_.push_state("MainMenu");

    while (running_ && !render_service.should_close() && !state_manager_.is_empty()) {
        float delta_time = render_service.get_frame_time();

        // Emit frame update event
        event_manager_.emit(FrameUpdateEvent(delta_time));

        // Update services
        service_manager_.update(delta_time);

        // Process events
        event_manager_.process_events();

        // Update state manager
        state_manager_.update(delta_time);

        // Render frame
        render_service.begin_frame();

        // Let states handle their own rendering
        state_manager_.render();

        render_service.end_frame();

        // Handle state input
        state_manager_.handle_input();
    }
}

void Application::shutdown() {
    running_ = false;
    service_manager_.shutdown();
}

void Application::setup_ecs() {
    // Load ECS component types from shared library
    if (!system_loader_.load_components_from_so("build/lib/libECS.so", ecs_registry_)) {
        throw std::runtime_error("Failed to load ECS components from build/lib/libECS.so");
    }

    // Get the component factory from the loaded library
    component_factory_ = system_loader_.get_factory();
    if (!component_factory_) {
        throw std::runtime_error("Failed to get component factory from loaded library");
    }

    
    // Create local player entity using factory methods
    local_player_entity_ = ecs_registry_.spawn_entity();
    component_factory_->create_component<position>(ecs_registry_, local_player_entity_, 100.0f, 300.0f);
    component_factory_->create_component<velocity>(ecs_registry_, local_player_entity_, 0.0f, 0.0f);
    component_factory_->create_component<collider>(ecs_registry_, local_player_entity_, PLAYER_WIDTH, PLAYER_HEIGHT);
    component_factory_->create_component<drawable>(ecs_registry_, local_player_entity_, PLAYER_WIDTH, PLAYER_HEIGHT, 255, 255, 255, 255);
    component_factory_->create_component<sprite>(ecs_registry_, local_player_entity_, "assets/REAPER_ICON.png", 128.0f, 64.0f, 1.0f, 1.0f);
}

void Application::setup_event_handlers() {
    // Handle ESC key to quit
    event_manager_.subscribe<KeyPressEvent>([this](const KeyPressEvent& e) {
        if (e.key == KEY_ESCAPE) {
            running_ = false;
        }
    });

    // Handle network connection
    event_manager_.subscribe<NetworkConnectedEvent>([this](const NetworkConnectedEvent& e) {
        local_player_id_ = e.player_id;
    });

    // Handle network disconnection
    event_manager_.subscribe<NetworkDisconnectedEvent>([this](const NetworkDisconnectedEvent& e) {
        running_ = false;
    });
}

void Application::update_ecs_systems(float delta_time) {
    // Update event-driven ECS systems
    input_system_->update(ecs_registry_, delta_time);
    network_system_->update(ecs_registry_, delta_time);
}

void Application::update_traditional_ecs_systems(float delta_time) {
    // Update all loaded systems through the DLLoader
    system_loader_.update_all_systems(ecs_registry_, delta_time);
    
}

void Application::setup_game_states() {
    // Register all game states with factory functions
    state_manager_.register_state<MainMenuState>("MainMenu");

    // Register Settings state with Application pointer using custom factory
    state_manager_.register_state_with_factory("Settings", [this]() -> std::shared_ptr<IGameState> {
        return std::make_shared<SettingsState>(this);
    });

    // Register Lobby state with Application pointer using custom factory
    state_manager_.register_state_with_factory("Lobby", [this]() -> std::shared_ptr<IGameState> {
        return std::make_shared<LobbyState>(this);
    });

    // Register Waiting Lobby state with Application pointer using custom factory
    state_manager_.register_state_with_factory("WaitingLobby", [this]() -> std::shared_ptr<IGameState> {
        return std::make_shared<WaitingLobbyState>(this);
    });

    // Register InGame state with Application pointer using custom factory
    state_manager_.register_state_with_factory("InGame", [this]() -> std::shared_ptr<IGameState> {
        return std::make_shared<InGameState>(this);
    });
}