#include "server_game_instance.hpp"
#include "Entity/Systems/NetworkSyncSystem/NetworkSyncSystem.hpp"
#include <iostream>
#include <stdexcept>

namespace RType::Network {

ServerGameInstance::ServerGameInstance()
    : component_factory_(nullptr), running_(false), initialized_(false), tickrate_(60) {
}

ServerGameInstance::~ServerGameInstance() {
    stop();
}

bool ServerGameInstance::initialize(std::shared_ptr<UdpServer> server) {
    if (initialized_) {
        std::cerr << "ServerGameInstance already initialized" << std::endl;
        return false;
    }

    server_ = server;

    try {
        setup_ecs();
        load_server_systems();
        
        initialized_ = true;
        std::cout << "ServerGameInstance initialized successfully" << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "ServerGameInstance initialization failed: " << e.what() << std::endl;
        return false;
    }
}

bool ServerGameInstance::start(int tickrate) {
    if (!initialized_) {
        std::cerr << "ServerGameInstance not initialized. Call initialize() first." << std::endl;
        return false;
    }

    if (running_) {
        std::cerr << "ServerGameInstance is already running" << std::endl;
        return false;
    }

    tickrate_ = tickrate;
    tick_duration_ = std::chrono::milliseconds(1000 / tickrate_);
    running_ = true;

    // Start game loop in separate thread
    game_thread_ = std::thread(&ServerGameInstance::game_loop, this);

    std::cout << "ServerGameInstance started with " << tickrate_ << " ticks/sec" << std::endl;
    return true;
}

void ServerGameInstance::stop() {
    if (running_) {
        running_ = false;
        
        if (game_thread_.joinable()) {
            game_thread_.join();
        }
        
        std::cout << "ServerGameInstance stopped" << std::endl;
    }
}

void ServerGameInstance::update(float delta_time) {
    if (!initialized_ || !running_) {
        return;
    }

    // Update all loaded ECS systems
    system_loader_.update_all_systems(ecs_registry_, delta_time);
}

void ServerGameInstance::setup_ecs() {
    // Load ECS component types from shared library
    if (!system_loader_.load_components_from_so("build/lib/libECS.so", ecs_registry_)) {
        throw std::runtime_error("Failed to load ECS components from build/lib/libECS.so");
    }

    // Get the component factory from the loaded library
    component_factory_ = system_loader_.get_factory();
    if (!component_factory_) {
        throw std::runtime_error("Failed to get component factory from loaded library");
    }

    std::cout << "Server ECS components loaded successfully" << std::endl;
}

void ServerGameInstance::load_server_systems() {
    // Load server-side systems (exclude client-only systems like drawing, input)
    std::vector<std::string> server_systems = {
        "lib/systems/libposition_system.so",
        "lib/systems/libcollision_system.so",
        "lib/systems/libgame_LifeTime.so",
        // Skip libgame_Draw.so - server doesn't need rendering
        "lib/systems/libanimation_system.so",  // Needed for state tracking
        // Skip libgame_Control.so - server doesn't handle direct input
        "lib/systems/libgame_EnemyAI.so",
        "lib/systems/libgame_EnemyCleanup.so",
        "lib/systems/libgame_EnemySpawnSystem.so",
        "lib/systems/libgame_NetworkSyncSystem.so",
        "lib/systems/libgame_Spawn.so",
        "lib/systems/libgame_Shoot.so",
        "lib/systems/libgame_Health.so"
    };

    for (const auto& system_path : server_systems) {
        try {
            system_loader_.load_system_from_so(system_path);
            std::cout << "Loaded server system: " << system_path << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Failed to load system " << system_path << ": " << e.what() << std::endl;
            // Continue loading other systems
        }
    }

    // Set up NetworkSyncSystem with server reference if it was loaded
    // TODO: Implement get_system_by_name in DLLoader to properly configure NetworkSyncSystem
    // auto network_sync_system = system_loader_.get_system_by_name("NetworkSyncSystem");
    // if (network_sync_system && server_) {
    //     // Try to cast and set server reference
    //     auto* network_sync = dynamic_cast<NetworkSyncSystem*>(network_sync_system);
    //     if (network_sync) {
    //         network_sync->set_server(server_);
    //         std::cout << "NetworkSyncSystem configured with server reference" << std::endl;
    //     }
    // }

    std::cout << "Server systems loaded successfully" << std::endl;
}

void ServerGameInstance::game_loop() {
    auto last_tick = std::chrono::high_resolution_clock::now();
    std::cout << "Server game loop started (" << tickrate_ << " ticks/sec)" << std::endl;

    while (running_) {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tick);
        
        if (elapsed >= tick_duration_) {
            last_tick = now;
            
            // Calculate delta time in seconds
            float delta_time = static_cast<float>(tick_duration_.count()) / 1000.0f;
            
            // Update game simulation
            update(delta_time);
        }
        
        // Sleep for a short time to prevent excessive CPU usage
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    std::cout << "Server game loop stopped" << std::endl;
}

} // namespace RType::Network