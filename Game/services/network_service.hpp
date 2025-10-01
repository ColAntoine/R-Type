#pragma once

#include "core/service_manager.hpp"
#include "core/event_manager.hpp"
#include "core/events.hpp"
#include "client.hpp"
#include "protocol.hpp"
#include <thread>
#include <atomic>
#include <chrono>

class NetworkService : public IService {
    private:
        EventManager* event_manager_;
        std::unique_ptr<UDPClient> client_;
        std::atomic<bool> connected_{false};
        std::atomic<int> local_player_id_{0};

        // Connection settings
        std::string server_ip_ = "127.0.0.1";
        int server_port_ = 8080;

    public:
        NetworkService(EventManager* event_manager);

        void initialize() override;
        void shutdown() override;
        void update(float delta_time) override;

        // Connection management
        bool connect_to_server(const std::string& ip, int port);
        void disconnect();
        bool is_connected() const;
        int get_local_player_id() const;

        // Send messages
        void send_player_position(float x, float y, float vx, float vy);

    private:
        void send_connection_request();
        void handle_network_message(const std::string& message);
        void handle_server_accept(const RType::Protocol::PacketHeader& header, const uint8_t* payload);
        void handle_position_update(const RType::Protocol::PacketHeader& header, const uint8_t* payload);
        void handle_player_join(const RType::Protocol::PacketHeader& header, const uint8_t* payload);
        void handle_player_leave(const RType::Protocol::PacketHeader& header, const uint8_t* payload);
        void handle_entity_create(const RType::Protocol::PacketHeader& header, const uint8_t* payload);
        void handle_entity_destroy(const RType::Protocol::PacketHeader& header, const uint8_t* payload);
        uint32_t get_current_timestamp();
};