#include "GameServer.hpp"
#include "Network/NetworkManager.hpp"
#include "Protocol/MessageQueue.hpp"
#include "ServerECS/ServerECS.hpp"
#include "ECS/Utils/Console.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <raylib.h>

GameServer::GameServer(bool display, bool windowed, float scale)
    : port_(8080)
    , running_(false)
    , display_(display)
    , windowed_(windowed)
    , scale_(scale)
{
}

GameServer::~GameServer()
{
    if (running_) {
        shutdown();
    }
}

bool GameServer::init()
{
    std::cout << Console::green("[GameServer] ") << "Initializing..." << std::endl;

    // Create io_context
    io_context_ = std::make_unique<asio::io_context>();
    network_manager_ = std::make_unique<RType::Network::NetworkManager>();
    if (!network_manager_->initialize(port_)) {
        std::cerr << Console::red("[GameServer] ") << "Failed to initialize NetworkManager" << std::endl;
        return false;
    }
    message_queue_ = std::make_unique<RType::Network::MessageQueue>();

    // Attach message queue to server
    auto server = network_manager_->get_server();
    if (!server) {
        std::cerr << Console::red("[GameServer] ") << "No UDP server available" << std::endl;
        return false;
    }
    RType::Network::MessageQueue* msg_ptr = message_queue_.get();
    server->set_message_queue(msg_ptr);

    // Create ServerECS (game logic)
    server_ecs_ = std::make_unique<RType::Network::ServerECS>();
    server_ecs_->set_message_queue(msg_ptr);
    // Provide a callback so ServerECS can send packets back to specific sessions
    server_ecs_->set_send_callback([server](const std::string& session_id, const std::vector<uint8_t>& packet) {
        if (!server) return;
        server->send_to_client(session_id, reinterpret_cast<const char*>(packet.data()), packet.size());
    });

    // Provide the UDP server pointer to ServerECS/Multiplayer so it can trigger broadcasts directly
    server_ecs_->set_udp_server(server.get());

    server_ecs_->init("lib/libECS.so");

    // Load logic systems
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libposition_system.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libcollision_system.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_Control.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_Shoot.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_GravitySys.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_EnemyCleanup.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_EnemyAI.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_Health.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_LifeTime.so", DLLoader::LogicSystem);
    server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_EnemySpawnSystem.so", DLLoader::LogicSystem);

    if (display_) {
        RenderManager::instance().init("R-Type Server", scale_, !windowed_);
        // Load render systems for display
        server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libanimation_system.so", DLLoader::RenderSystem);
        server_ecs_->GetDLLoader().load_system_from_so("build/lib/systems/libgame_Draw.so", DLLoader::RenderSystem);
    }

    // Start network with worker threads
    if (!network_manager_->start(2)) {
        std::cerr << Console::red("[GameServer] ") << "Failed to start NetworkManager" << std::endl;
        return false;
    }

    running_ = true;
    std::cout << Console::green("[GameServer] ") << "Initialized successfully on port " << port_ << std::endl;
    return true;
}

void GameServer::run()
{
    std::cout << Console::green("[GameServer] ") << "Starting game loop..." << std::endl;
    run_tick_loop();
}

void GameServer::run_tick_loop()
{
    std::cout << Console::cyan("[GameServer] ") << "Entering authoritative tick loop (30Hz)" << std::endl;
    const std::chrono::milliseconds tick_duration(33); // ~30Hz

    while (running_) {
        auto tick_start = std::chrono::steady_clock::now();

        // Process incoming network packets and convert to ECS components
        server_ecs_->process_packets();

        // Run ECS systems (loader-managed)
        server_ecs_->tick(0.033f);

        if (display_) {
            RenderManager::instance().begin_frame();
            // Update render systems
            server_ecs_->GetDLLoader().update_all_systems(server_ecs_->GetRegistry(), 0.033f, DLLoader::RenderSystem);
            RenderManager::instance().end_frame();
            if (WindowShouldClose()) {
                running_ = false;
            }
        }

        // Sleep until next tick
        auto elapsed = std::chrono::steady_clock::now() - tick_start;
        if (elapsed < tick_duration) {
            std::this_thread::sleep_for(tick_duration - elapsed);
        }
    }

    std::cout << Console::yellow("[GameServer] ") << "Tick loop ended" << std::endl;
}

void GameServer::update(float delta)
{
    // Individual update call (if needed outside the main loop)
    (void)delta;
}

void GameServer::shutdown()
{
    std::cout << Console::yellow("[GameServer] ") << "Shutting down..." << std::endl;
    running_ = false;

    if (network_manager_) {
        network_manager_->stop();
    }

    server_ecs_.reset();
    message_queue_.reset();
    network_manager_.reset();
    io_context_.reset();

    if (display_) {
        RenderManager::instance().shutdown();
    }

    std::cout << Console::green("[GameServer] ") << "Shutdown complete" << std::endl;
}
