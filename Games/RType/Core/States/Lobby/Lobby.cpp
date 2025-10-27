#include "Lobby.hpp"

#include <string>
#include "Constants.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/Components/InputField.hpp"
#include "ECS/Zipper.hpp"

void Lobby::enter()
{
    std::cout << "[Lobby] Entering state" << std::endl;

    _systemLoader.load_components_from_so("build/lib/libECS.so", _registry);
    _systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);

    setup_ui();
    _initialized = true;
}

void Lobby::exit()
{
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

void Lobby::update(float delta_time)
{
    if  (!_initialized)
        return;

    _systemLoader.update_all_systems(_registry, delta_time, DLLoader::LogicSystem);
}

void Lobby::setup_ui()
{
    std::cout << "[Lobby] Setting up UI" << std::endl;

    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    // Create bar to split the panel
    auto bar = PanelBuilder()
        .at(renderManager.scalePosX(40), renderManager.scalePosY(5))
        .size(20, renderManager.scaleSizeH(90))
        .border(20, WHITE)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto barEntity = _registry.spawn_entity();
    _registry.add_component(barEntity, UI::UIComponent(bar));

    auto playersPlaceHolder = TextBuilder()
        .at(renderManager.scalePosX(7), renderManager.scalePosY(6))
        .text("Players:")
        .textColor(Color({255, 255, 0, 255}))
        .fontSize(renderManager.scaleSizeW(3))
    .build(winInfos.getWidth(), winInfos.getHeight());

    auto playersPlaceHolderEntity = _registry.spawn_entity();
    _registry.add_component(playersPlaceHolderEntity, UI::UIComponent(playersPlaceHolder));

    auto placeholderName = TextBuilder()
        .at(renderManager.scalePosX(7), renderManager.scalePosY(12))
        .text("PLAYER NAME !")
        .fontSize(renderManager.scaleSizeW(2))
    .build(winInfos.getWidth(), winInfos.getHeight());

    auto placeHolderNameEntity = _registry.spawn_entity();
    _registry.add_component(placeHolderNameEntity, UI::UIComponent(placeholderName));

    auto placeholderName2 = TextBuilder()
        .at(renderManager.scalePosX(7), renderManager.scalePosY(16))
        .text("PLAYER NAME 2 !")
        .fontSize(renderManager.scaleSizeW(2))
    .build(winInfos.getWidth(), winInfos.getHeight());

    auto placeHolderName2Entity = _registry.spawn_entity();
    _registry.add_component(placeHolderName2Entity, UI::UIComponent(placeholderName2));

    auto readyButton = ButtonBuilder()
        .at(renderManager.scalePosX(23), renderManager.scalePosY(85))
        .size(renderManager.scaleSizeW(15), renderManager.scaleSizeH(8))
        .text("READY")
        .green()
        .textColor(WHITE)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, WHITE)
        .onClick([this]() {
            // set the player to ready
        })
    .build(winInfos.getWidth(), winInfos.getHeight());

    auto readyButtonEntity = _registry.spawn_entity();
    _registry.add_component(readyButtonEntity, UI::UIComponent(readyButton));

    auto backButton = ButtonBuilder()
        .at(renderManager.scalePosX(7), renderManager.scalePosY(85))
        .size(renderManager.scaleSizeW(15), renderManager.scaleSizeH(8))
        .text("BACK")
        .red()
        .textColor(WHITE)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, WHITE)
        .onClick([this]() {
            if (_stateManager) {
                _stateManager->pop_state();
                _stateManager->push_state("Connection");
            }
        })
    .build(winInfos.getWidth(), winInfos.getHeight());

    auto backButtonEntity = _registry.spawn_entity();
    _registry.add_component(backButtonEntity, UI::UIComponent(backButton));


    auto gameStatus = TextBuilder()
        .at(renderManager.scalePosX(45), renderManager.scalePosY(50))
        .text("Waiting for the players to be ready...")
        .fontSize(renderManager.scaleSizeW(3))
        .red()
    .build(winInfos.getWidth(), winInfos.getHeight());

    auto gameStatusEntity = _registry.spawn_entity();
    _registry.add_component(gameStatusEntity, UI::UIComponent(gameStatus));

    auto playerUpdate = TextBuilder()
        .at(renderManager.scalePosX(60), renderManager.scalePosY(55))
        .text("Player 2 just joined !")
        .fontSize(renderManager.scaleSizeW(2))
        .textColor(Color({255, 255, 0, 255}))
    .build(winInfos.getWidth(), winInfos.getHeight());

    auto playerUpdateEntity = _registry.spawn_entity();
    _registry.add_component(playerUpdateEntity, UI::UIComponent(playerUpdate));
}
