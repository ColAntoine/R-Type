#pragma once

#include "Core/AGameCore.hpp"
#include "Core/States/GameStateManager.hpp"
#include <memory>
#include <asio.hpp>

// Forward declarations
namespace RType::Network {
    class NetworkManager;
    class MessageQueue;
    class ServerECS;
}

class ServerLobby;

class GameServer : public AGameCore {
public:
    GameServer(bool display = false, bool windowed = false, float scale = 1.0f);
    ~GameServer();

    bool init() override;
    void run() override;
    void update(float delta) override;
    void shutdown() override;

private:
    void run_tick_loop();
    void register_states();
    void start_game(); // Transition from lobby to game

    std::unique_ptr<asio::io_context> io_context_;
    std::unique_ptr<RType::Network::NetworkManager> network_manager_;
    std::unique_ptr<RType::Network::MessageQueue> message_queue_;
    std::unique_ptr<RType::Network::ServerECS> server_ecs_;

    GameStateManager state_manager_;
    std::shared_ptr<ServerLobby> lobby_state_;

    uint16_t port_;
    bool running_;
    bool display_;
    bool windowed_;
    float scale_;
    bool game_started_;
    // Whether ECS logic/render systems have been loaded into the DLLoader
    bool systems_loaded_{false};
};
