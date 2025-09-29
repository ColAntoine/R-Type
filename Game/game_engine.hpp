#pragma once

#include "client.hpp"
#include "game_window.hpp"
#include "ecs/registry.hpp"
#include "ecs/components.hpp"
#include "ecs/entity.hpp"
#include "../Network/protocol.hpp"
#include <unordered_map>
#include <string>

class GameEngine {
    private:
        registry ecs_registry;
        GameWindow* window;
        UDPClient* client;
        entity player_entity;
        bool initialized;
        int local_player_id;
        std::unordered_map<int, entity> remote_players;
        std::unordered_map<uint32_t, entity> enemies; // Track server enemies

        void setup_entities();
        void update_systems(float dt);
        void render_ui();
        void send_player_position();
        void handle_network_message(const std::string& message);
        void handle_binary_message(const Protocol::PacketHeader& header, const uint8_t* payload);
        void update_remote_player(int player_id, float x, float y);
        entity create_remote_player(int player_id, float x, float y);
        void remove_remote_player(int player_id);
        void update_enemy(uint32_t enemy_id, float x, float y, float vx, float vy);
        entity create_enemy(uint32_t enemy_id, float x, float y);

    public:
        GameEngine();
        ~GameEngine();

        bool initialize(GameWindow* game_window, UDPClient* network_client);
        void run();
        void shutdown();

        bool is_running() const;
};