#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

#include "ECS/UI/UIBuilder.hpp"

#include "Constants.hpp"
#include "Core/Server/Protocol/Protocol.hpp"
#include <vector>

class Lobby : public AGameState {

    struct ReadyButton: UI::UIButton {};
    struct PlayerListContainerTag {};  // Tag for the container holding player entries
    struct PlayerEntryTag { uint32_t player_id; };  // Tag for individual player entry UI elements

    public:
        Lobby() = default;
        ~Lobby() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;
        void update(float delta_time) override;

        void setup_ui() override;

        std::string get_name() const override { return "Lobby"; }

        virtual bool blocks_update() const override { return false; }
        virtual bool blocks_render() const override { return false; }

        void update_player_list(const std::vector<RType::Protocol::PlayerInfo>& players);

    private:
        void rebuild_player_ui(const std::vector<RType::Protocol::PlayerInfo>& players);
        void toggle_ready_state();
        void update_ready_button();
        void on_back_clicked();

        std::vector<RType::Protocol::PlayerInfo> current_players_;
        bool is_ready_ = false;
        bool game_start_ = false;
        entity ready_button_entity_;
};