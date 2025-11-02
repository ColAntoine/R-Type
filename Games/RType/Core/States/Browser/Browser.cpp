#include "Browser.hpp"

#include <string>
#include "Constants.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/Components/InputField.hpp"
#include "Core/Client/Network/ClientService.hpp"
#include "Core/Client/Network/NetworkService.hpp"
#include "ECS/Zipper.hpp"
#include "Core/Server/Protocol/Protocol.hpp"
#include "ECS/Messaging/MessagingManager.hpp"
#include "ECS/Messaging/Events/Event.hpp"

#include "UI/ThemeManager.hpp"
#include "UI/Components/GlitchButton.hpp"

void Browser::enter()
{
    std::cout << "[Browser] Entering state" << std::endl;

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
    auto* net_mgr = RType::Network::get_network_manager();
    if (net_mgr) {
        // INSTANCE_CREATED -> reconnect and enter lobby (creation flow)
        // Capture the state manager pointer instead of `this` to avoid invoking a dangling Browser pointer
        auto *stateMgr = this->_stateManager;
        net_mgr->get_player_handler().set_instance_created_callback([stateMgr](uint16_t port) {
            if (port == 0) {
                std::cout << "[Browser] Instance creation failed or denied by server" << std::endl;
                return;
            }
            std::cout << "[Browser] Received instance port " << port << ", reconnecting..." << std::endl;
            auto client = RType::Network::get_client();
            if (!client) return;
            std::string server_ip = client->get_server_ip();
            std::string player_name = client->get_player_name();

            // If we have an active session on the front server, notify it we are leaving
            uint32_t session_token = client->get_session_token();
            if (session_token != 0) {
                try {
                    std::cout << "[Browser] Sending disconnect to front server (session=" << session_token << ") before reconnecting to instance" << std::endl;
                    client->send_disconnect(session_token);
                } catch (...) {
                    // best-effort: ignore send failures
                }
            }

            // Fully close the current connection so we stop receiving front-server broadcasts
            client->disconnect();

            auto accept = client->connect(server_ip, port, player_name);
            if (accept) {
                // Defer UI state changes to the main thread via the event bus
                auto& bus = MessagingManager::instance().get_event_bus();
                Event ev("INSTANCE_CONNECTED_UI");
                ev.set<uint16_t>("port", port);
                ev.set<uint8_t>("multi", static_cast<uint8_t>(accept->multi_instance));
                bus.emit_deferred(ev);
            } else {
                std::cerr << "[Browser] Failed to connect to instance on port " << port << std::endl;
            }
        });

        // INSTANCE_LIST -> update UI list
        net_mgr->get_player_handler().set_instance_list_callback([this](const std::vector<RType::Protocol::InstanceInfo>& list) {
            this->rebuild_instance_ui(list);
        });
    }
    _initialized = true;

    // Subscribe to deferred instance-connected event to perform UI transitions on main thread
    auto& bus = MessagingManager::instance().get_event_bus();
    _instanceConnectedCallbackId = bus.subscribe("INSTANCE_CONNECTED_UI", [this](const Event& ev) {
        if (!this->_stateManager) return;

        // Otherwise (multi!=0) return to MainMenu.
        bool is_instance_server = true;
        try {
            uint8_t multi = ev.get<uint8_t>("multi");
            is_instance_server = (multi == 0);
        } catch (...) {
            // missing data; fallback to MainMenu
            is_instance_server = false;
        }

        this->_stateManager->pop_state();
        if (is_instance_server) {
            this->_stateManager->push_state("Lobby");
        } else {
            this->_stateManager->push_state("MainMenu");
        }
    });
}

void Browser::exit()
{
    auto &eventBus = MessagingManager::instance().get_event_bus();
    eventBus.unsubscribe(_uiEventCallbackId);
    // Unregister instance-created callback
    auto* net_mgr = RType::Network::get_network_manager();
    if (net_mgr) {
        net_mgr->get_player_handler().set_instance_created_callback(nullptr);
        net_mgr->get_player_handler().set_instance_list_callback(nullptr);
    }
    _initialized = false;
    // Unsubscribe deferred UI event
    auto& bus = MessagingManager::instance().get_event_bus();
    bus.unsubscribe(_instanceConnectedCallbackId);
}

void Browser::pause()
{
    std::cout << "[Browser] Pausing state" << std::endl;
}

void Browser::resume()
{
    std::cout << "[Browser] Resuming state" << std::endl;
}

void Browser::setup_ui()
{
    std::cout << "[Browser] Setting up UI" << std::endl;
    auto &theme = ThemeManager::instance().getTheme();

    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    // Title
    auto title = TextBuilder()
        .at(renderManager.scalePosX(40), renderManager.scalePosY(10))
        .text("SERVER BROWSER")
        .fontSize(renderManager.scaleSizeW(4))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto titleEntity = _registry.spawn_entity();
    _registry.add_component(titleEntity, UI::UIComponent(title));

    auto createButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(20), renderManager.scalePosY(25))
        .size(renderManager.scaleSizeW(25), renderManager.scaleSizeH(10))
        .text("CREATE INSTANCE")
        .color(theme.secondaryButtonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.secondaryButtonColors.border)
        .onClick([this]() {
            // Reuse join_room_callback to send REQUEST_INSTANCE (create-on-demand)
            this->join_room_callback();
        })
        .neonColors(theme.secondaryButtonColors.neonColor, theme.secondaryButtonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto createButtonEntity = _registry.spawn_entity();
    _registry.add_component(createButtonEntity, UI::UIComponent(createButton));

    // Instruction text when there are no instances
    auto infoLabel = TextBuilder()
        .at(renderManager.scalePosX(20), renderManager.scalePosY(40))
        .text("No open instances. Create one or wait for others to create.")
        .fontSize(renderManager.scaleSizeW(2))
        .textColor(theme.secondaryTextColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto infoEntity = _registry.spawn_entity();
    _registry.add_component(infoEntity, UI::UIComponent(infoLabel));
    _instanceEntities.push_back(infoEntity);

    // Back Button
    auto backButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(40), renderManager.scalePosY(85))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("BACK")
        .color(theme.exitButtonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.exitButtonColors.border)
        .onClick([this]() {
            // Disconnect before going back
            auto client = RType::Network::get_client();
            if (client) {
                uint32_t player_id = client->get_session_token();
                if (player_id != 0) {
                    std::cout << "[Browser] Sending disconnect for player " << player_id << std::endl;
                    client->send_disconnect(player_id);
                }
                client->disconnect();
            }
            if (this->_stateManager) {
                this->_stateManager->pop_state(); // pop Browser
                this->_stateManager->push_state("Connection"); // go back to Connection tab
            }
        })
        .neonColors(theme.exitButtonColors.neonColor, theme.exitButtonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto backButtonEntity = _registry.spawn_entity();
    _registry.add_component(backButtonEntity, UI::UIComponent(backButton));
}

void Browser::join_room_callback()
{
    std::cout << "[Browser] Join room clicked" << std::endl;
    auto client = RType::Network::get_client();
    if (!client) {
        std::cerr << "[Browser] No client available to send instance request" << std::endl;
        return;
    }
    RType::Protocol::PacketBuilder builder;
    builder.begin_packet(static_cast<uint8_t>(RType::Protocol::SystemMessage::REQUEST_INSTANCE));
    auto buf = builder.finalize();
    client->send_packet(reinterpret_cast<const char*>(buf.data()), buf.size());
}

void Browser::rebuild_instance_ui(const std::vector<RType::Protocol::InstanceInfo>& list) {
    for (auto e : _instanceEntities) {
        _registry.kill_entity(e);
    }
    _instanceEntities.clear();

    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();
    auto &theme = ThemeManager::instance().getTheme();

    if (list.empty()) {
        auto infoLabel = TextBuilder()
            .at(renderManager.scalePosX(20), renderManager.scalePosY(40))
            .text("No open instances. Create one or wait for others to create.")
            .fontSize(renderManager.scaleSizeW(2))
            .textColor(theme.secondaryTextColor)
            .build(winInfos.getWidth(), winInfos.getHeight());
        auto infoEntity = _registry.spawn_entity();
        _registry.add_component(infoEntity, UI::UIComponent(infoLabel));
        _instanceEntities.push_back(infoEntity);
        return;
    }

    for (size_t i = 0; i < list.size(); ++i) {
        const auto &inst = list[i];
        std::string labelText = "Instance " + std::to_string(i+1) + " - Port: " + std::to_string(inst.port);
        auto label = TextBuilder()
            .at(renderManager.scalePosX(20), renderManager.scalePosY(40 + i*10))
            .text(labelText.c_str())
            .fontSize(renderManager.scaleSizeW(2))
            .textColor(theme.secondaryTextColor)
            .build(winInfos.getWidth(), winInfos.getHeight());
        auto labelEntity = _registry.spawn_entity();
        _registry.add_component(labelEntity, UI::UIComponent(label));
        _instanceEntities.push_back(labelEntity);

        // Join button
            // Capture state manager pointer, avoid capturing `this` to prevent dangling access in callback
            auto *stateMgrJoin = this->_stateManager;
            auto joinButton = GlitchButtonBuilder()
            .at(renderManager.scalePosX(60), renderManager.scalePosY(40 + i*10))
            .size(renderManager.scaleSizeW(15), renderManager.scaleSizeH(6))
            .text("JOIN")
            .color(theme.secondaryButtonColors.normal)
            .textColor(theme.textColor)
            .fontSize(renderManager.scaleSizeW(2))
            .border(2, theme.secondaryButtonColors.border)
            .onClick([port = inst.port, stateMgrJoin]() {
                // Connect directly to instance. First, gracefully leave the front server so
                // we do not receive front-server broadcasts while connected to the instance.
                auto client = RType::Network::get_client();
                if (!client) return;
                std::string server_ip = client->get_server_ip();
                std::string player_name = client->get_player_name();

                uint32_t session_token = client->get_session_token();
                if (session_token != 0) {
                    try {
                        std::cout << "[Browser] Sending disconnect to front server (session=" << session_token << ") before joining instance" << std::endl;
                        client->send_disconnect(session_token);
                    } catch (...) {
                        // ignore
                    }
                }
                client->disconnect();

                auto accept = client->connect(server_ip, port, player_name);
                if (accept) {
                    // Defer UI state changes to main thread
                    auto& bus = MessagingManager::instance().get_event_bus();
                    Event ev("INSTANCE_CONNECTED_UI");
                    ev.set<uint16_t>("port", port);
                    ev.set<uint8_t>("multi", static_cast<uint8_t>(accept->multi_instance));
                    bus.emit_deferred(ev);
                }
            })
            .neonColors(theme.secondaryButtonColors.neonColor, theme.secondaryButtonColors.neonGlowColor)
            .glitchParams(2.0f, 8.0f, true)
            .build(winInfos.getWidth(), winInfos.getHeight());
        auto btnEntity = _registry.spawn_entity();
        _registry.add_component(btnEntity, UI::UIComponent(joinButton));
        _instanceEntities.push_back(btnEntity);
    }
}