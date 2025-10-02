#include "application.hpp"

bool Application::initialize(const std::string& server_ip, int server_port) {
    try {
        // Register ECS components
        setup_ecs();

        // Load ECS systems from shared libraries
        system_loader_.load_system_from_so("lib/systems/libposition_system.so");
        system_loader_.load_system_from_so("lib/systems/libcollision_system.so");
        system_loader_.load_system_from_so("lib/systems/liblifetime_system.so");
        system_loader_.load_system_from_so("lib/systems/libdraw_system.so");

        // Register services
        service_manager_.register_service<InputService>(&event_manager_);
        service_manager_.register_service<NetworkService>(&event_manager_);
        service_manager_.register_service<RenderService>(&event_manager_);

        // Initialize services
        service_manager_.initialize();

        // Create ECS systems
        input_system_ = std::make_unique<InputSystem>(&event_manager_, local_player_entity_);
        network_system_ = std::make_unique<NetworkSystem>(&event_manager_,
            &service_manager_.get_service<NetworkService>());

        // Subscribe to application events
        setup_event_handlers();

        // Connect to server
        auto& network_service = service_manager_.get_service<NetworkService>();
        if (!network_service.connect_to_server(server_ip, server_port)) {
            std::cerr << "Failed to connect to server" << std::endl;
            return false;
        }

        running_ = true;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Application initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void Application::run() {
    if (!running_) {
        std::cerr << "Application not initialized" << std::endl;
        return;
    }

    auto& render_service = service_manager_.get_service<RenderService>();

    std::cout << "Application started. Use Arrow Keys to move, ESC to quit." << std::endl;

    while (running_ && !render_service.should_close()) {
        float delta_time = render_service.get_frame_time();

        // Emit frame update event
        event_manager_.emit(FrameUpdateEvent(delta_time));

        // Update services
        service_manager_.update(delta_time);

        // Process events
        event_manager_.process_events();

        // Update ECS systems
        update_ecs_systems(delta_time);

        // Update traditional ECS systems
        update_traditional_ecs_systems(delta_time);

        // Render frame
        render_frame();
    }

    std::cout << "Application stopped" << std::endl;
}

void Application::shutdown() {
    running_ = false;
    service_manager_.shutdown();
}

void Application::setup_ecs() {
    // Load ECS component types from shared library
    if (!system_loader_.load_components_from_so("lib/libECS.so", ecs_registry_)) {
        throw std::runtime_error("Failed to load ECS components from lib/libECS.so");
    }

    // Create local player entity
    local_player_entity_ = ecs_registry_.spawn_entity();
    ecs_registry_.add_component(local_player_entity_, position(100.0f, 300.0f));
    ecs_registry_.add_component(local_player_entity_, velocity(0.0f, 0.0f));
    ecs_registry_.add_component(local_player_entity_, drawable(40.0f, 40.0f, 255, 0, 0, 255)); // Red
    ecs_registry_.add_component(local_player_entity_, controllable(200.0f));
    ecs_registry_.add_component(local_player_entity_, collider(40.0f, 40.0f));
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
        std::cout << "Connected to server. Player ID: " << local_player_id_ << std::endl;
    });

    // Handle network disconnection
    event_manager_.subscribe<NetworkDisconnectedEvent>([this](const NetworkDisconnectedEvent& e) {
        std::cout << "Disconnected from server" << std::endl;
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

void Application::render_frame() {
    auto& render_service = service_manager_.get_service<RenderService>();
    auto& network_service = service_manager_.get_service<NetworkService>();

    // Begin frame
    render_service.begin_frame();

    // Render entities through the draw system
    system_loader_.update_system_by_name("DrawSystem", ecs_registry_, 0.0f);

    // Render UI
    std::ostringstream status;
    status << "FPS: " << render_service.get_fps()
           << " | Player " << local_player_id_
           << " | Connected: " << (network_service.is_connected() ? "Yes" : "No");

    if (auto* pos_arr = ecs_registry_.get_if<position>();
        pos_arr && pos_arr->size() > static_cast<size_t>(local_player_entity_)) {
        auto& player_pos = (*pos_arr)[static_cast<size_t>(local_player_entity_)];
        status << " | You: (" << (int)player_pos.x << ", " << (int)player_pos.y << ")";
    }

    render_service.render_ui(status.str());

    // End frame
    render_service.end_frame();
}