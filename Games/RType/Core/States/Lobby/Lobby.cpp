#include "Lobby.hpp"

#include <string>
#include "Constants.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/Zipper.hpp"
#include "Core/Client/Network/NetworkService.hpp"
#include "Core/Client/Network/ClientService.hpp"

#include "UI/ThemeManager.hpp"
#include "UI/Components/GlitchButton.hpp"

#if defined(_MSC_VER)
  #define ATTR_MAYBE_UNUSED [[maybe_unused]]
#else
  #define ATTR_MAYBE_UNUSED __attribute__((unused))
#endif

void Lobby::enter()
{
    std::cout << "[Lobby] Entering state" << std::endl;

    #ifdef _WIN32
        const std::string ecsLib = "build/lib/libECS.dll";
        const std::string uiSys = "build/lib/systems/librender_UISystem.dll";
    #else
        const std::string ecsLib = "build/lib/libECS.so";
        const std::string uiSys = "build/lib/systems/librender_UISystem.so";
    #endif

    _systemLoader->load_components(ecsLib, _registry);
    _systemLoader->load_system(uiSys, ILoader::RenderSystem);

    setup_ui();
    subscribe_to_ui_event();

    // Register callback to update player list when CLIENT_LIST is received
    auto* net_mgr = RType::Network::get_network_manager();
    if (net_mgr) {
        net_mgr->get_player_handler().set_client_list_callback(
            [this](const std::vector<RType::Protocol::PlayerInfo>& players) {
                this->update_player_list(players);
            }
        );

        // Register callback to detect game start
        net_mgr->get_player_handler().set_game_start_callback(
            [this]() {
                std::cout << "[Lobby] Game starting! Setting flag..." << std::endl;
                this->game_start_ = true;
            }
        );

        // Register callback for chat messages
        net_mgr->get_chat_handler().set_chat_callback(
            [this](const std::vector<ChatEntry>& messages) {
                this->on_chat_message(messages);
            }
        );
    }

    _initialized = true;
}

void Lobby::exit()
{
    auto &eventBus = MessagingManager::instance().get_event_bus();
    eventBus.unsubscribe(_uiEventCallbackId);
    // Clear the callbacks when leaving Lobby
    auto* net_mgr = RType::Network::get_network_manager();
    if (net_mgr) {
        net_mgr->get_player_handler().set_client_list_callback(nullptr);
        net_mgr->get_player_handler().set_game_start_callback(nullptr);
        net_mgr->get_chat_handler().set_chat_callback(nullptr);
    }
    _initialized = false;
}

void Lobby::pause()
{
    std::cout << "[Lobby] Pausing state" << std::endl;
}

void Lobby::resume()
{
    std::cout << "[Lobby] Resuming state" << std::endl;
}

void Lobby::update(ATTR_MAYBE_UNUSED float delta_time)
{
    if (game_start_ && _stateManager) {
        _stateManager->pop_state();
        _stateManager->push_state("InGameMultiplayer");
        _stateManager->push_state("InGameHud");
        _stateManager->push_state("InGameBackground");
    }
}

void Lobby::setup_ui()
{
    std::cout << "[Lobby] Setting up UI" << std::endl;

    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    auto &theme = ThemeManager::instance().getTheme();

    // Create bar to split the panel
    auto bar = PanelBuilder()
        .at(renderManager.scalePosX(40), renderManager.scalePosY(5))
        .size(20, renderManager.scaleSizeH(90))
        .border(20, theme.panelBorderColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto barEntity = _registry.spawn_entity();
    _registry.add_component(barEntity, UI::UIComponent(bar));

    auto playersPlaceHolder = TextBuilder()
        .at(renderManager.scalePosX(7), renderManager.scalePosY(6))
        .text("Players:")
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(3))
    .build(winInfos.getWidth(), winInfos.getHeight());

    auto playersPlaceHolderEntity = _registry.spawn_entity();
    _registry.add_component(playersPlaceHolderEntity, UI::UIComponent(playersPlaceHolder));

    // Tag the container for player list updates
    _registry.register_component<PlayerListContainerTag>();
    _registry.add_component<PlayerListContainerTag>(playersPlaceHolderEntity, PlayerListContainerTag());

    auto readyButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(23), renderManager.scalePosY(85))
        .size(renderManager.scaleSizeW(15), renderManager.scaleSizeH(8))
        .text("READY")
        .color(theme.secondaryButtonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.secondaryButtonColors.border)
        .neonColors(theme.secondaryButtonColors.neonColor, theme.secondaryButtonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            toggle_ready_state();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    ready_button_entity_ = _registry.spawn_entity();
    _registry.add_component(ready_button_entity_, UI::UIComponent(readyButton));

    auto backButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(7), renderManager.scalePosY(85))
        .size(renderManager.scaleSizeW(15), renderManager.scaleSizeH(8))
        .text("BACK")
        .color(theme.exitButtonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.exitButtonColors.border)
        .neonColors(theme.exitButtonColors.neonColor, theme.exitButtonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->on_back_clicked();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto backButtonEntity = _registry.spawn_entity();
    _registry.add_component(backButtonEntity, UI::UIComponent(backButton));

    auto gameStatus = TextBuilder()
        .at(renderManager.scalePosX(45), renderManager.scalePosY(50))
        .textColor(theme.textColor)
        .text("Waiting for the players to be ready...")
        .fontSize(renderManager.scaleSizeW(3))
    .build(winInfos.getWidth(), winInfos.getHeight());

    auto gameStatusEntity = _registry.spawn_entity();
    _registry.add_component(gameStatusEntity, UI::UIComponent(gameStatus));

    // Chat Panel
    auto chatPanel = PanelBuilder()
        .at(renderManager.scalePosX(45), renderManager.scalePosY(10))
        .size(renderManager.scaleSizeW(50), renderManager.scaleSizeH(35))
        .border(2, theme.panelBorderColor)
        .backgroundColor(Color{30, 30, 40, 200})
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto chatPanelEntity = _registry.spawn_entity();
    _registry.add_component(chatPanelEntity, UI::UIComponent(chatPanel));
    _registry.register_component<ChatMessageTag>();

    // Chat title
    auto chatTitle = TextBuilder()
        .at(renderManager.scalePosX(46), renderManager.scalePosY(11))
        .text("Chat")
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto chatTitleEntity = _registry.spawn_entity();
    _registry.add_component(chatTitleEntity, UI::UIComponent(chatTitle));

    // Chat input field
    auto chatInput = UIBuilder<UI::UIInputField>()
        .at(renderManager.scalePosX(46), renderManager.scalePosY(42))
        .size(renderManager.scaleSizeW(48), renderManager.scaleSizeH(3))
        .placeholder("Type a message...")
        .textColor(theme.textColor)
        .placeholderColor(theme.secondaryTextColor)
        .fontSize(renderManager.scaleSizeW(1.5))
        .build(winInfos.getWidth(), winInfos.getHeight());

    chat_input_entity_ = _registry.spawn_entity();
    _registry.add_component(chat_input_entity_, UI::UIComponent(chatInput));
    _registry.register_component<ChatInputTag>();
    _registry.add_component<ChatInputTag>(chat_input_entity_, ChatInputTag());

    // Set up input field callback
    if (auto ui_comps = _registry.get_if<UI::UIComponent>(); ui_comps && ui_comps->has(chat_input_entity_)) {
        auto& ui_comp = (*ui_comps)[chat_input_entity_];
        if (auto* input_field = dynamic_cast<UI::UIInputField*>(ui_comp._ui_element.get())) {
            input_field->setOnEnterPressed([this](const std::string& text) {
                if (!text.empty()) {
                    this->send_chat_message(text);
                }
            });
        }
    }
}

void Lobby::update_player_list(const std::vector<RType::Protocol::PlayerInfo>& players) {
    current_players_ = players;
    rebuild_player_ui(players);
}

void Lobby::rebuild_player_ui(const std::vector<RType::Protocol::PlayerInfo>& players) {
    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    auto &theme = ThemeManager::instance().getTheme();

    // Clear existing player entry UI elements
    auto* ui_comps = _registry.get_if<UI::UIComponent>();
    auto* player_tags = _registry.get_if<PlayerEntryTag>();
    if (ui_comps && player_tags) {
        std::vector<entity> to_remove;
        for (auto [ui_comp, tag, idx] : zipper(*ui_comps, *player_tags)) {
            to_remove.push_back(static_cast<entity>(idx));
        }
        for (auto ent : to_remove) {
            _registry.kill_entity(ent);
        }
    }

    // Register component if not already done
    if (!player_tags) {
        _registry.register_component<PlayerEntryTag>();
    }

    // Display each player starting from Y position 12
    float startY = 12.0f;
    float spacing = 4.0f;

    for (size_t i = 0; i < players.size() && i < 8; ++i) {
        const auto& player = players[i];
        std::string name(player.name, strnlen(player.name, sizeof(player.name)));
        std::string ready_status = player.ready_state ? "(Ready)" : "(Not Ready)";
        Color name_color = player.ready_state ? theme.lobbyStateColors.ready : theme.lobbyStateColors.notReady;

        // Player name text
        auto playerNameText = TextBuilder()
            .at(renderManager.scalePosX(7), renderManager.scalePosY(startY + i * spacing))
            .text(name)
            .fontSize(renderManager.scaleSizeW(2))
            .textColor(name_color)
            .build(winInfos.getWidth(), winInfos.getHeight());

        auto playerNameEntity = _registry.spawn_entity();
        _registry.add_component(playerNameEntity, UI::UIComponent(playerNameText));
        _registry.add_component<PlayerEntryTag>(playerNameEntity, PlayerEntryTag{player.player_id});

        // Player status text (smaller, next to name)
        auto playerStatusText = TextBuilder()
            .at(renderManager.scalePosX(20 + name.length() * 0.6f), renderManager.scalePosY(startY + i * spacing))
            .text(" " + ready_status)
            .fontSize(renderManager.scaleSizeW(1.5))
            .textColor(theme.secondaryTextColor)
            .build(winInfos.getWidth(), winInfos.getHeight());

        auto playerStatusEntity = _registry.spawn_entity();
        _registry.add_component(playerStatusEntity, UI::UIComponent(playerStatusText));
        _registry.add_component<PlayerEntryTag>(playerStatusEntity, PlayerEntryTag{player.player_id});
    }
}

void Lobby::toggle_ready_state() {
    is_ready_ = !is_ready_;
    update_ready_button();

    auto client = RType::Network::get_client();
    if (!client) {
        return;
    }

    // Identify our own player id using the session token stored in the UDP client
    uint32_t player_id = 0;
    if (client) {
        player_id = client->get_session_token();
    }
    if (player_id == 0) {
        // Fallback: try first entry from server list (legacy behavior)
        if (!current_players_.empty()) player_id = current_players_[0].player_id;
        if (player_id == 0) std::cerr << "[Lobby] Warning: player_id is 0, cannot send ready state" << std::endl;
    }

    // Create ClientReady message
    RType::Protocol::ClientReady ready_msg;
    ready_msg.player_id = player_id;
    ready_msg.ready_state = is_ready_ ? 1 : 0;

    // Determine which message type to use
    auto msg_type = is_ready_
        ? RType::Protocol::SystemMessage::CLIENT_READY
        : RType::Protocol::SystemMessage::CLIENT_UNREADY;

    // Create and send packet
    try {
        RType::Protocol::PacketBuilder builder;
        builder.begin_packet(static_cast<uint8_t>(msg_type));
        builder.add_struct(ready_msg);
        auto buf = builder.finalize();

        client->send_packet(reinterpret_cast<const char*>(buf.data()), buf.size());

    } catch (const std::exception& e) {
        std::cerr << "[Lobby] Error sending ready packet: " << e.what() << std::endl;
    }
}

void Lobby::update_ready_button() {
    auto& theme = ThemeManager::instance().getTheme();
    auto* ui_comps = _registry.get_if<UI::UIComponent>();
    if (!ui_comps || !ui_comps->has(ready_button_entity_)) {
        std::cerr << "[Lobby] Failed to get ready button UI component" << std::endl;
        return;
    }

    auto& ui_comp = (*ui_comps)[ready_button_entity_];
    auto* button = dynamic_cast<UI::UIButton*>(ui_comp._ui_element.get());
    if (!button) {
        std::cerr << "[Lobby] Ready button entity doesn't contain a button" << std::endl;
        return;
    }

    // Update button text and color based on ready state
    if (is_ready_) {
        button->setText("UNREADY");
        button->getStyle().setNormalColor(theme.exitButtonColors.normal);
        button->getStyle().setHoveredColor(ColorCalculator::lighter(theme.exitButtonColors.normal, 20));
    } else {
        button->setText("READY");
        button->getStyle().setNormalColor(theme.secondaryButtonColors.normal);
        button->getStyle().setHoveredColor(ColorCalculator::lighter(theme.secondaryButtonColors.normal, 20));
    }
}

void Lobby::on_back_clicked() {
    // Disconnect the client before leaving
    auto client = RType::Network::get_client();
    if (client) {
        uint32_t player_id = client->get_session_token();
        if (player_id != 0) {
            std::cout << "[Lobby] Sending disconnect for player " << player_id << std::endl;
            client->send_disconnect(player_id);
        }
        client->disconnect();
    }
    if (_stateManager) {
        // Pop current state (Lobby)
        _stateManager->pop_state();
        _stateManager->push_state("Connection");
    }
}

std::string Lobby::get_player_name(uint32_t player_id) const {
    for (const auto& player : current_players_) {
        if (player.player_id == player_id) {
            return std::string(player.name, strnlen(player.name, sizeof(player.name)));
        }
    }
    return "Player " + std::to_string(player_id);
}

void Lobby::on_chat_message(const std::vector<ChatEntry>& messages) {
    // Resolve player names from the player list and update entries
    std::vector<ChatEntry> resolved_messages = messages;
    for (auto& entry : resolved_messages) {
        // Update player name from player list if available
        std::string actual_name = get_player_name(entry.player_id);
        if (actual_name != entry.player_name) {
            entry.player_name = actual_name;
        }
    }
    current_chat_messages_ = resolved_messages;
    rebuild_chat_ui(resolved_messages);
}

void Lobby::rebuild_chat_ui(const std::vector<ChatEntry>& messages) {
    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();
    auto &theme = ThemeManager::instance().getTheme();

    // Clear existing chat message entities
    auto* ui_comps = _registry.get_if<UI::UIComponent>();
    auto* chat_tags = _registry.get_if<ChatMessageTag>();
    if (ui_comps && chat_tags) {
        std::vector<entity> to_remove;
        for (auto [ui_comp, tag, idx] : zipper(*ui_comps, *chat_tags)) {
            to_remove.push_back(static_cast<entity>(idx));
        }
        for (auto ent : to_remove) {
            _registry.kill_entity(ent);
        }
    }

    // Register component if not already done
    if (!chat_tags) {
        _registry.register_component<ChatMessageTag>();
    }

    // Display chat messages from bottom to top
    float startY = 42.5f;  // Start lower, within the visible chat panel area
    float spacing = 2.3f;
    int displayed_count = 0;
    const int max_display = 12;

    // Display from end of messages (most recent last)
    size_t start_idx = messages.size() > max_display ? messages.size() - max_display : 0;
    
    for (size_t i = start_idx; i < messages.size(); ++i) {
        const auto& msg = messages[i];
        if (displayed_count >= max_display) break;

        float y_pos = startY - (max_display - displayed_count - 1) * spacing;

        // Format: "PlayerName: message"
        std::string formatted = msg.player_name + ": " + msg.message;

        auto chatMessageText = TextBuilder()
            .at(renderManager.scalePosX(46.5f), renderManager.scalePosY(y_pos))
            .text(formatted)
            .fontSize(renderManager.scaleSizeW(1.3))
            .textColor(theme.textColor)
            .build(winInfos.getWidth(), winInfos.getHeight());

        auto chatMessageEntity = _registry.spawn_entity();
        _registry.add_component(chatMessageEntity, UI::UIComponent(chatMessageText));
        _registry.add_component<ChatMessageTag>(chatMessageEntity, ChatMessageTag());

        displayed_count++;
    }
}

void Lobby::send_chat_message(const std::string& message) {
    // Get the client and player ID
    auto client = RType::Network::get_client();
    if (!client) {
        std::cerr << "[Lobby] Client not available for sending chat" << std::endl;
        return;
    }

    uint32_t player_id = client->get_session_token();
    if (player_id == 0) {
        if (!current_players_.empty()) player_id = current_players_[0].player_id;
        if (player_id == 0) {
            std::cerr << "[Lobby] Cannot determine player ID for chat" << std::endl;
            return;
        }
    }

    // Create and send CLIENT_CHAT message
    try {
        RType::Protocol::ChatMessage chat_msg;
        chat_msg.player_id = player_id;
        strncpy(chat_msg.message, message.c_str(), sizeof(chat_msg.message) - 1);
        chat_msg.message[sizeof(chat_msg.message) - 1] = '\0';

        RType::Protocol::PacketBuilder builder;
        builder.begin_packet(static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_CHAT));
        builder.add_struct(chat_msg);
        auto buf = builder.finalize();

        client->send_packet(reinterpret_cast<const char*>(buf.data()), buf.size());

        // Clear the input field
        if (auto ui_comps = _registry.get_if<UI::UIComponent>(); ui_comps && ui_comps->has(chat_input_entity_)) {
            auto& ui_comp = (*ui_comps)[chat_input_entity_];
            if (auto* input_field = dynamic_cast<UI::UIInputField*>(ui_comp._ui_element.get())) {
                input_field->setText("");
            }
        }

        // Message will appear when server broadcasts it back to all clients
        std::cout << "[Lobby] Sent chat message: " << message << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[Lobby] Error sending chat message: " << e.what() << std::endl;
    }
}
