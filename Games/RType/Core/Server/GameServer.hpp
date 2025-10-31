#pragma once

#include "Core/AGameCore.hpp"
#include "Core/States/GameStateManager.hpp"
#include <memory>
#include <asio.hpp>
#include <vector>
#include "Core/Server/Protocol/Protocol.hpp"
#include <thread>
#include <mutex>
#include <map>

// Forward declarations
namespace RType::Network {
    class NetworkManager;
    class MessageQueue;
    class ServerECS;
}

class ServerLobby;

class GameServer : public AGameCore {
public:
    GameServer(bool display = false, bool windowed = false, float scale = 1.0f, int maxLobbies = 0, int maxPlayers = 2, bool is_machine_made = false);
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
    int max_lobbies_;
    int max_players_;
    bool is_machine_made_;
    // Instance management
    std::mutex instances_mtx_;
    int active_instances_{0};
    uint16_t next_instance_offset_{0}; // offsets from base port_
    std::map<uint16_t, std::thread> instance_threads_; // port -> thread
    std::vector<RType::Protocol::InstanceInfo> instances_;
    // Auto-shutdown for instance servers (spawned with max_lobbies_ == 0)
    // When the connected client count reaches zero, we wait a small grace period
    // before terminating the instance to avoid flapping.
    bool zero_clients_timer_active_{false};
    std::chrono::steady_clock::time_point zero_clients_since_{};
    std::chrono::milliseconds zero_clients_grace_{5000}; // 5s grace period

    // Allow caller to override the bind port before init()
    void set_port(uint16_t p) { port_ = p; }
    // Handle client request to spawn instance
    void handle_instance_request(const std::string &session_id);
    void send_instance_created(const std::string &session_id, uint16_t port);
    void broadcast_instance_list();
};
