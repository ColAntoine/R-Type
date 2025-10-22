#include "GameServer.hpp"
#include "Network/NetworkManager.hpp"
#include "Protocol/MessageQueue.hpp"
#include "ServerECS/ServerECS.hpp"
#include "Utils/Console.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Zipper.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <raylib.h>

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

    // Initialize Raylib window for server display
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(1024, 768, "R-Type Server - Waiting for clients...");
    SetTargetFPS(60);

    if (!IsWindowReady()) {
        std::cerr << Console::red("[GameServer] ") << "Failed to initialize Raylib window" << std::endl;
        return false;
    }

    std::cout << Console::green("[GameServer] ") << "Raylib window initialized" << std::endl;

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

    while (running_ && !WindowShouldClose()) {
        auto tick_start = std::chrono::steady_clock::now();

        // Process incoming network packets and convert to ECS components
        server_ecs_->process_packets();

        // Run ECS systems (loader-managed)
        server_ecs_->tick(0.033f);

        // Render the server view
        BeginDrawing();
        render_game_state();
        EndDrawing();

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

    // Close Raylib window
    if (IsWindowReady()) {
        CloseWindow();
    }

    std::cout << Console::green("[GameServer] ") << "Shutdown complete" << std::endl;
}

void GameServer::render_game_state() {
    // Background
    ClearBackground({6, 8, 10, 255});

    // Grid overlay
    Color grid_color = {0, 40, 64, 30};
    int gw = 48;
    for (int x = 0; x < GetScreenWidth(); x += gw) {
        DrawLine(x, 0, x, GetScreenHeight(), grid_color);
    }
    for (int y = 0; y < GetScreenHeight(); y += gw) {
        DrawLine(0, y, GetScreenWidth(), y, grid_color);
    }

    // Server info
    DrawText("R-Type Server", 10, 10, 30, {0, 229, 255, 255});
    DrawText(TextFormat("Port: %d", port_), 10, 50, 20, {150, 150, 150, 255});

    // Get and render all player positions
    if (!server_ecs_) return;

    auto& reg = server_ecs_->GetRegistry();
    auto* positions = reg.get_if<position>();
    
    if (!positions || positions->size() == 0) {
        DrawText("Waiting for clients to connect...", 10, 90, 20, {200, 200, 200, 255});
        return;
    }

    // Render all player entities as colored squares
    int player_count = 0;
    Color player_colors[] = {
        {0, 229, 255, 255},   // Cyan
        {255, 100, 200, 255}, // Pink
        {100, 255, 100, 255}, // Green
        {255, 200, 100, 255}, // Orange
        {200, 100, 255, 255}, // Purple
        {255, 255, 100, 255}, // Yellow
        {100, 200, 255, 255}, // Light Blue
        {255, 150, 150, 255}  // Light Red
    };

    for (auto [pos, ent] : zipper(*positions)) {
        Color color = player_colors[player_count % 8];
        
        // Draw player square (40x40 like client)
        DrawRectangle(
            static_cast<int>(pos.x),
            static_cast<int>(pos.y),
            40,
            40,
            color
        );
        
        // Draw border
        DrawRectangleLines(
            static_cast<int>(pos.x),
            static_cast<int>(pos.y),
            40,
            40,
            WHITE
        );

        // Draw player ID above square
        DrawText(
            TextFormat("P%d", player_count + 1),
            static_cast<int>(pos.x) + 5,
            static_cast<int>(pos.y) - 20,
            16,
            color
        );

        player_count++;
    }

    // Draw player count
    DrawText(TextFormat("Connected Players: %d", player_count), 10, 90, 20, {0, 229, 255, 255});
}
