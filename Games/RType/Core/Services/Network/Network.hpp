#pragma once

#include "Core/Services/ServiceManager.hpp"
#include "Core/EventManager.hpp"
#include "Core/Events.hpp"
#include "Core/Client/Client.hpp"
#include "protocol.hpp"
#include <thread>
#include <atomic>
#include <chrono>

class NetworkService : public IService {
    private:
        EventManager* event_manager_;
        std::unique_ptr<UDPClient> client_;
        std::atomic<bool> connected_{false};
        std::atomic<bool> connection_confirmed_{false};  // Set when SERVER_ACCEPT received
        std::atomic<int> local_player_id_{0};

        // Connection settings
        std::string server_ip_ = "127.0.0.1";
        int server_port_ = 8080;
        std::string player_name_ = "Player";

    public:
        NetworkService(EventManager* event_manager);

        void initialize() override;
        void shutdown() override;
        void update(float delta_time) override;

        // Connection management
        bool connect_to_server(const std::string& ip, int port, const std::string& player_name = "Player");
        void disconnect();
        bool is_connected() const;
        int get_local_player_id() const;

        // Send messages
        void send_player_position(float x, float y, float vx, float vy);
        void send_player_shoot(float start_x, float start_y, float dir_x, float dir_y, uint8_t weapon_type);
        void send_entity_destroy(uint32_t entity_network_id);
        void send_ready_signal(bool ready = true);

    private:
        void send_connection_request();
        void handle_network_message(const std::string& message);
        void handle_server_accept(const RType::Protocol::PacketHeader& header, const uint8_t* payload);
        void handle_client_list(const RType::Protocol::PacketHeader& header, const uint8_t* payload);
        void handle_start_game(const RType::Protocol::PacketHeader& header, const uint8_t* payload);
        void handle_position_update(const RType::Protocol::PacketHeader& header, const uint8_t* payload);
        void handle_player_join(const RType::Protocol::PacketHeader& header, const uint8_t* payload);
        void handle_player_leave(const RType::Protocol::PacketHeader& header, const uint8_t* payload);
        void handle_entity_create(const RType::Protocol::PacketHeader& header, const uint8_t* payload);
        void handle_entity_update(const RType::Protocol::PacketHeader& header, const uint8_t* payload);
        void handle_entity_destroy(const RType::Protocol::PacketHeader& header, const uint8_t* payload);
        void handle_player_shoot(const RType::Protocol::PacketHeader& header, const uint8_t* payload);
        uint32_t get_current_timestamp();
};