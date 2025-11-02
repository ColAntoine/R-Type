#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

#include "ECS/UI/UIBuilder.hpp"

#include "Constants.hpp"
#include "Core/Server/Protocol/Protocol.hpp"
#include "Core/Client/Network/Handlers/ChatHandler.hpp"
#include <vector>

class Lobby : public AGameState {

    struct ReadyButton: UI::UIButton {};
    struct PlayerListContainerTag {};  // Tag for the container holding player entries
    struct PlayerEntryTag { uint32_t player_id; };  // Tag for individual player entry UI elements
    struct ChatMessageTag {};  // Tag for chat message display entities
    struct ChatInputTag {};    // Tag for chat input field

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

        // Update the player list display with new data
        void update_player_list(const std::vector<RType::Protocol::PlayerInfo>& players);

        // Handle chat messages
        void on_chat_message(const std::vector<ChatEntry>& messages);
        void send_chat_message(const std::string& message);

    private:
        void rebuild_player_ui(const std::vector<RType::Protocol::PlayerInfo>& players);
        void rebuild_chat_ui(const std::vector<ChatEntry>& messages);
        void toggle_ready_state();
        void update_ready_button();
        void on_back_clicked();
        std::string get_player_name(uint32_t player_id) const;

        std::vector<RType::Protocol::PlayerInfo> current_players_;
        bool is_ready_ = false;
        bool game_start_ = false;
        entity ready_button_entity_;
        entity chat_input_entity_;
        std::vector<ChatEntry> current_chat_messages_;
};