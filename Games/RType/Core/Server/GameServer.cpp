#include "GameServer.hpp"
#include "Network/NetworkManager.hpp"
#include "Protocol/MessageQueue.hpp"
#include "ServerECS/ServerECS.hpp"
#include "ServerECS/Communication/Multiplayer.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "Core/Server/States/ServerLobby.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <raylib.h>
#include <random>

GameServer::GameServer(bool display, bool windowed, float scale)
    : port_(8080)
    , running_(false)
    , display_(display)
    , windowed_(windowed)
    , scale_(scale)
    , game_started_(false)
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
    std::cout << "[GameServer] Initializing..." << std::endl;

    // Create io_context
    io_context_ = std::make_unique<asio::io_context>();
    network_manager_ = std::make_unique<RType::Network::NetworkManager>();
    if (!network_manager_->initialize(port_)) {
        std::cerr << "[GameServer] Failed to initialize NetworkManager" << std::endl;
        return false;
    }
    message_queue_ = std::make_unique<RType::Network::MessageQueue>();

    // Attach message queue to server
    auto server = network_manager_->get_server();
    if (!server) {
        std::cerr << "[GameServer] No UDP server available" << std::endl;
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

    // Register game start callback with server (this will override the one in Multiplayer)
    // We need to call both start_game() and ensure players are spawned
    server->set_game_start_callback([this]() {
        this->start_game();
    });

    server_ecs_->init("lib/libECS.so");

    // Load logic systems (they won't run until game starts)
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
        
        // Register and setup lobby state
        register_states();
        state_manager_.push_state("ServerLobby");
    }

    // Start network with worker threads
    if (!network_manager_->start(2)) {
        std::cerr << "[GameServer] Failed to start NetworkManager" << std::endl;
        return false;
    }

    running_ = true;
    std::cout << "[GameServer] Initialized successfully on port " << port_ << std::endl;
    return true;
}

void GameServer::run()
{
    std::cout << "[GameServer] Starting game loop..." << std::endl;
    run_tick_loop();
}

void GameServer::run_tick_loop()
{
    std::cout << "[GameServer] Entering authoritative tick loop (30Hz)" << std::endl;
    const std::chrono::milliseconds tick_duration(33); // ~30Hz

    while (running_) {
        auto tick_start = std::chrono::steady_clock::now();

        // Process incoming network packets and convert to ECS components
        server_ecs_->process_packets();

        // Only run game systems if game has started
        if (game_started_) {
            server_ecs_->tick(0.033f);
        }

        if (display_) {
            RenderManager::instance().begin_frame();
            
            // Update state manager (handles lobby UI)
            state_manager_.update(0.033f);
            
            // Render state manager (lobby or game UI)
            state_manager_.render();
            
            // If game started, also render game entities
            if (game_started_) {
                server_ecs_->GetDLLoader().update_all_systems(server_ecs_->GetRegistry(), 0.033f, DLLoader::RenderSystem);
            }
            
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

    std::cout << "[GameServer] Tick loop ended" << std::endl;
}

void GameServer::update(float delta)
{
    // Individual update call (if needed outside the main loop)
    (void)delta;
}

void GameServer::shutdown()
{
    std::cout << "[GameServer] Shutting down..." << std::endl;
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

    std::cout << "[GameServer] Shutdown complete" << std::endl;
}

void GameServer::register_states()
{
    std::cout << "[GameServer] Registering states..." << std::endl;
    
    // Create lobby state with UDP server reference
    auto server = network_manager_->get_server();
    state_manager_.register_state_with_factory("ServerLobby", [this, server]() -> std::shared_ptr<IGameState> {
        lobby_state_ = std::make_shared<ServerLobby>(server.get());
        return lobby_state_;
    });
}

void GameServer::start_game()
{
    std::cout << "[GameServer] Starting game! Transitioning from lobby..." << std::endl;

    game_started_ = true;
    
    // Generate and set random seed for deterministic gameplay
    std::random_device rd;
    unsigned int game_seed = rd();
    server_ecs_->GetRegistry().set_random_seed(game_seed);

    std::cout << "[GameServer] Generated game seed: " << game_seed << std::endl;

    // Broadcast the seed to all clients
    auto server = network_manager_->get_server();
    if (server) {
        RType::Protocol::GameSeed seed_msg;
        seed_msg.seed = game_seed;
        
        auto packet = RType::Protocol::create_packet(
            static_cast<uint8_t>(RType::Protocol::GameMessage::GAME_SEED),
            seed_msg
        );
        
        server->broadcast(reinterpret_cast<const char*>(packet.data()), packet.size());
        std::cout << "[GameServer] Game seed broadcasted to all clients" << std::endl;
    }
    
    // Spawn all players (was previously done in Multiplayer callback that got overwritten)
    if (server_ecs_->GetMultiplayer()) {
        server_ecs_->GetMultiplayer()->spawn_all_players();
    }
    
    // Mark lobby as game started (for clients to know)
    if (lobby_state_) {
        lobby_state_->set_game_started(true);
    }
    
    // Clear lobby UI if in display mode
    if (display_) {
        state_manager_.clear_states();
    }

    std::cout << "[GameServer] Game started! ECS systems now active." << std::endl;
}

