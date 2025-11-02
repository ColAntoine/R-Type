#pragma once

#include <string>
#include <vector>
#include <utility>

#include "Protocol/Protocol.hpp"
#include "ECS/Entity.hpp"

namespace RType::Network {

    class ServerECS;
    class UdpServer;

    class Lobby {
        public:
            explicit Lobby(ServerECS &ecs);
            ~Lobby();

            void set_udp_server(UdpServer* server);

            // Handlers for lobby-related packets
            void handle_client_connect(const std::string &session_id, const std::vector<char> &payload);
            void handle_client_disconnect(const std::string &session_id, const std::vector<char> &payload);
            void handle_client_ready(const std::string &session_id, const std::vector<char> &payload);
            void handle_client_unready(const std::string &session_id, const std::vector<char> &payload);
            void handle_client_chat(const std::string &session_id, const std::vector<char> &payload);

        private:
            ServerECS &ecs_;
            UdpServer* udp_server_{nullptr};

            // Helpers used by lobby flows
            std::pair<float,float> choose_spawn_position();
            void send_server_accept(const std::string &session_id, uint32_t token, float x, float y);
    };

} // namespace RType::Network
