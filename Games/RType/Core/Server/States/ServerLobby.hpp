#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "Constants.hpp"
#include "Core/Server/Protocol/Protocol.hpp"
#include "Core/Server/Network/UDPServer.hpp"
#include <vector>
#include <memory>

class ServerLobby : public AGameState {

    struct PlayerListContainerTag {};  // Tag for the container holding player entries
    struct PlayerEntryTag { uint32_t player_id; };  // Tag for individual player entry UI elements

    public:
        ServerLobby(RType::Network::UdpServer* udp_server);
        ~ServerLobby() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;
        void update(float delta_time) override;

        void setup_ui() override;

        std::string get_name() const override { return "ServerLobby"; }

        virtual bool blocks_update() const override { return false; }
        virtual bool blocks_render() const override { return false; }

        void update_player_list();

        // Flag to track if game has started
        bool has_game_started() const { return game_started_; }
        void set_game_started(bool started) { game_started_ = started; }

    private:
        void rebuild_player_ui();

        RType::Network::UdpServer* udp_server_;
        std::vector<RType::Protocol::PlayerInfo> current_players_;
        bool game_started_ = false;
};
