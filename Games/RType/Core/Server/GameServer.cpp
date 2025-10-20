#include "GameServer.hpp"
#include "Network/NetworkManager.hpp"
#include "Protocol/MessageQueue.hpp"
#include "ServerECS/ServerECS.hpp"
#include "Utils/Console.hpp"
#include <iostream>
#include <thread>
#include <chrono>

GameServer::GameServer()
    : port_(8080)
    , running_(false)
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

    // Create NetworkManager
    network_manager_ = std::make_unique<RType::Network::NetworkManager>();
    if (!network_manager_->initialize(port_)) {
        std::cerr << Console::red("[GameServer] ") << "Failed to initialize NetworkManager" << std::endl;
        return false;
    }

    // Create MessageQueue
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

    std::cout << Console::green("[GameServer] ") << "Shutdown complete" << std::endl;
}
