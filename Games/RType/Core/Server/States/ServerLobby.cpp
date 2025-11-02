#include "ServerLobby.hpp"

#include <string>
#include <iostream>
#include "Constants.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/Zipper.hpp"

ServerLobby::ServerLobby(RType::Network::UdpServer* udp_server)
    : udp_server_(udp_server)
{
}

void ServerLobby::enter()
{
    std::cout << "[ServerLobby] Entering lobby state" << std::endl;

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

    _initialized = true;
}

void ServerLobby::exit()
{
    std::cout << "[ServerLobby] Exiting lobby state" << std::endl;
    _initialized = false;
}

void ServerLobby::pause()
{
    std::cout << "[ServerLobby] Pausing lobby state" << std::endl;
}

void ServerLobby::resume()
{
    std::cout << "[ServerLobby] Resuming lobby state" << std::endl;
}

void ServerLobby::update(float delta_time)
{
    static float update_timer = 0.0f;
    update_timer += delta_time;

    if (update_timer >= 0.5f) { // Update twice per second
        update_player_list();
        update_timer = 0.0f;
    }
}

void ServerLobby::setup_ui()
{
    std::cout << "[ServerLobby] Setting up lobby UI" << std::endl;

    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    // Title
    auto titleText = TextBuilder()
        .at(renderManager.scalePosX(30), renderManager.scalePosY(5))
        .text("R-Type Server - Lobby")
        .textColor(Color({255, 255, 0, 255}))
        .fontSize(renderManager.scaleSizeW(4))
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto titleEntity = _registry.spawn_entity();
    _registry.add_component(titleEntity, UI::UIComponent(titleText));

    auto bar = PanelBuilder()
        .at(renderManager.scalePosX(40), renderManager.scalePosY(15))
        .size(20, renderManager.scaleSizeH(70))
        .border(20, WHITE)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto barEntity = _registry.spawn_entity();
    _registry.add_component(barEntity, UI::UIComponent(bar));

    // Players list header
    auto playersPlaceHolder = TextBuilder()
        .at(renderManager.scalePosX(7), renderManager.scalePosY(12))
        .text("Connected Players:")
        .textColor(Color({255, 255, 0, 255}))
        .fontSize(renderManager.scaleSizeW(3))
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto playersPlaceHolderEntity = _registry.spawn_entity();
    _registry.add_component(playersPlaceHolderEntity, UI::UIComponent(playersPlaceHolder));

    // Tag the container for player list updates
    _registry.register_component<PlayerListContainerTag>();
    _registry.add_component<PlayerListContainerTag>(playersPlaceHolderEntity, PlayerListContainerTag());

    // Status text
    auto statusText = TextBuilder()
        .at(renderManager.scalePosX(45), renderManager.scalePosY(50))
        .text("Waiting for players to connect and ready up...")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(Color({255, 100, 100, 255}))
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto statusEntity = _registry.spawn_entity();
    _registry.add_component(statusEntity, UI::UIComponent(statusText));
}

void ServerLobby::update_player_list()
{
    if (!udp_server_) return;

    auto client_list = udp_server_->generate_player_list();

    // Convert to vector for easier handling
    std::vector<RType::Protocol::PlayerInfo> players;
    for (uint8_t i = 0; i < client_list.player_count; ++i) {
        players.push_back(client_list.players[i]);
    }

    // Only rebuild UI if the list has changed
    if (players.size() != current_players_.size()) {
        current_players_ = players;
        rebuild_player_ui();
    } else {
        bool changed = false;
        for (size_t i = 0; i < players.size(); ++i) {
            if (players[i].ready_state != current_players_[i].ready_state) {
                changed = true;
                break;
            }
        }
        if (changed) {
            current_players_ = players;
            rebuild_player_ui();
        }
    }
}

void ServerLobby::rebuild_player_ui()
{
    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

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

    if (!player_tags) {
        _registry.register_component<PlayerEntryTag>();
    }

    // Display each player starting from Y position 18
    float startY = 18.0f;
    float spacing = 4.0f;

    for (size_t i = 0; i < current_players_.size() && i < 8; ++i) {
        const auto& player = current_players_[i];
        std::string name(player.name, strnlen(player.name, sizeof(player.name)));
        std::string ready_status = player.ready_state ? " [READY]" : " [NOT READY]";
        Color name_color = player.ready_state ? GREEN : WHITE;

        // Player name and status text combined
        std::string display_text = name + ready_status;

        auto playerText = TextBuilder()
            .at(renderManager.scalePosX(7), renderManager.scalePosY(startY + i * spacing))
            .text(display_text)
            .fontSize(renderManager.scaleSizeW(2.5))
            .textColor(name_color)
            .build(winInfos.getWidth(), winInfos.getHeight());

        auto playerEntity = _registry.spawn_entity();
        _registry.add_component(playerEntity, UI::UIComponent(playerText));
        _registry.add_component<PlayerEntryTag>(playerEntity, PlayerEntryTag{player.player_id});
    }

    // Display count
    std::string count_text = "Players: " + std::to_string(current_players_.size()) + " / 8";
    auto countText = TextBuilder()
        .at(renderManager.scalePosX(7), renderManager.scalePosY(startY + 8 * spacing + 2))
        .text(count_text)
        .fontSize(renderManager.scaleSizeW(2))
        .textColor(Color({200, 200, 200, 255}))
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto countEntity = _registry.spawn_entity();
    _registry.add_component(countEntity, UI::UIComponent(countText));
}
