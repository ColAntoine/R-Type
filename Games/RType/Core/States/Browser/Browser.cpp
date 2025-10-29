#include "Browser.hpp"

#include <string>
#include "Constants.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/Components/InputField.hpp"
#include "Core/Client/Network/ClientService.hpp"
#include "ECS/Zipper.hpp"

void Browser::enter()
{
    std::cout << "[Browser] Entering state" << std::endl;

    _systemLoader.load_components_from_so("build/lib/libECS.so", _registry);
    _systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);

    setup_ui();
    subscribe_to_ui_event();
    _initialized = true;
}

void Browser::exit()
{
    auto &eventBus = MessagingManager::instance().get_event_bus();
    eventBus.unsubscribe(_uiEventCallbackId);
    _initialized = false;
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

    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    // Title
    auto title = TextBuilder()
        .at(renderManager.scalePosX(40), renderManager.scalePosY(10))
        .text("SERVER BROWSER")
        .fontSize(renderManager.scaleSizeW(4))
        .textColor(WHITE)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto titleEntity = _registry.spawn_entity();
    _registry.add_component(titleEntity, UI::UIComponent(title));

    // Mock room #1
    auto room1Label = TextBuilder()
        .at(renderManager.scalePosX(20), renderManager.scalePosY(25))
        .text("#1 - Players: 2/4")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(WHITE)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto room1Entity = _registry.spawn_entity();
    _registry.add_component(room1Entity, UI::UIComponent(room1Label));

    // Join button for room #1
    auto joinButton = ButtonBuilder()
        .at(renderManager.scalePosX(60), renderManager.scalePosY(25))
        .size(renderManager.scaleSizeW(15), renderManager.scaleSizeH(8))
        .text("JOIN")
        .green()
        .textColor(WHITE)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, WHITE)
        .onClick([this]() {
            this->join_room_callback();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto joinButtonEntity = _registry.spawn_entity();
    _registry.add_component(joinButtonEntity, UI::UIComponent(joinButton));

    // Back Button
    auto backButton = ButtonBuilder()
        .at(renderManager.scalePosX(40), renderManager.scalePosY(85))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("BACK")
        .red()
        .textColor(WHITE)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, WHITE)
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
                this->_stateManager->pop_state();
            }
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto backButtonEntity = _registry.spawn_entity();
    _registry.add_component(backButtonEntity, UI::UIComponent(backButton));
}

void Browser::join_room_callback()
{
    std::cout << "[Browser] Join room clicked" << std::endl;
    if (_stateManager) {
        _stateManager->pop_state();
        _stateManager->push_state("Lobby");
    }
}