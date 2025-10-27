#include "Connection.hpp"

#include <string>
#include "Constants.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/Components/InputField.hpp"

void Connection::enter()
{
    std::cout << "[Connection] Entering state" << std::endl;

    _systemLoader.load_components_from_so("build/lib/libECS.so", _registry);
    _systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);

    setup_ui();
    _initialized = true;
}

void Connection::exit()
{
    _initialized = false;
}

void Connection::pause()
{
    std::cout << "[Connection] Pausing state" << std::endl;
}

void Connection::resume()
{
    std::cout << "[Connection] Resuming state" << std::endl;
}

void Connection::setup_ui()
{
    std::cout << "[Connection] Setting up UI" << std::endl;

    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    // Create bar to split the panel
    auto bar = PanelBuilder()
        .centered(renderManager.scalePosY(0))
        .size(20, renderManager.scaleSizeH(90))
        .border(20, WHITE)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto barEntity = _registry.spawn_entity();
    _registry.add_component(barEntity, UI::UIComponent(bar));

    // Left side: Input fields for IP and Port
    // IP Label
    auto ipLabel = TextBuilder()
        .at(renderManager.scalePosX(10), renderManager.scalePosY(13))
        .text("IP Address:")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(WHITE)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto ipLabelEntity = _registry.spawn_entity();
    _registry.add_component(ipLabelEntity, UI::UIComponent(ipLabel));

    // IP Input Field
    auto ipInput = InputBuilder()
        .at(renderManager.scalePosX(10), renderManager.scalePosY(18))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(6))
        .placeholder("Enter IP address")
        .backgroundColor(Color{50, 50, 50, 255})
        .text("127.0.0.1")
        .textColor(WHITE)
        .placeholderColor(Color{150, 150, 150, 255})
        .border(2, Color{200, 200, 200, 255})
        .fontSize(renderManager.scaleSizeW(2))
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto ipInputEntity = _registry.spawn_entity();
    _registry.add_component(ipInputEntity, UI::UIComponent(ipInput));

    // Port Label
    auto portLabel = TextBuilder()
        .at(renderManager.scalePosX(10), renderManager.scalePosY(28))
        .text("Port:")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(WHITE)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto portLabelEntity = _registry.spawn_entity();
    _registry.add_component(portLabelEntity, UI::UIComponent(portLabel));

    // Port Input Field
    auto portInput = InputBuilder()
        .at(renderManager.scalePosX(10), renderManager.scalePosY(33))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(6))
        .placeholder("Enter port")
        .backgroundColor(Color{50, 50, 50, 255})
        .textColor(WHITE)
        .placeholderColor(Color{150, 150, 150, 255})
        .border(2, Color{200, 200, 200, 255})
        .fontSize(renderManager.scaleSizeW(2))
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto portInputEntity = _registry.spawn_entity();
    _registry.add_component(portInputEntity, UI::UIComponent(portInput));

    // Back Button (Red)
    auto backButton = ButtonBuilder()
        .at(renderManager.scalePosX(63), renderManager.scalePosY(85))
        .size(renderManager.scaleSizeW(15), renderManager.scaleSizeH(8))
        .text("BACK")
        .red()
        .textColor(WHITE)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, WHITE)
        .onClick([this]() {
            // ? THIS IS PRINTED 2 times ?
            std::cout << "++++++++TEST++++++++++" << std::endl;
            if (this->_stateManager) {
                this->_stateManager->pop_state();
                this->_stateManager->pop_state();
                this->_stateManager->push_state("MainMenu");
            }
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto backButtonEntity = _registry.spawn_entity();
    _registry.add_component(backButtonEntity, UI::UIComponent(backButton));

    // Connection Button (Green)
    auto connectButton = ButtonBuilder()
        .at(renderManager.scalePosX(79), renderManager.scalePosY(85))
        .size(renderManager.scaleSizeW(15), renderManager.scaleSizeH(8))
        .text("CONNECT")
        .green()
        .textColor(WHITE)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, WHITE)
        .onClick([this]() {
            if (_stateManager) {
                _stateManager->pop_state();
                _stateManager->push_state("Lobby");
            }
            // Connection logic will be implemented here
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto connectButtonEntity = _registry.spawn_entity();
    _registry.add_component(connectButtonEntity, UI::UIComponent(connectButton));

    // right side: Input fields for IP and Port
    // IP Label
    auto nameLabel = TextBuilder()
        .at(renderManager.scalePosX(60), renderManager.scalePosY(13))
        .text("Name:")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(WHITE)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto nameLabelEntity = _registry.spawn_entity();
    _registry.add_component(nameLabelEntity, UI::UIComponent(nameLabel));

    // IP Input Field
    auto nameInput = InputBuilder()
        .at(renderManager.scalePosX(60), renderManager.scalePosY(18))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(6))
        .placeholder("Enter your name")
        .backgroundColor(Color{50, 50, 50, 255})
        .textColor(WHITE)
        .placeholderColor(Color{150, 150, 150, 255})
        .border(2, Color{200, 200, 200, 255})
        .fontSize(renderManager.scaleSizeW(2))
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto nameInputEntity = _registry.spawn_entity();
    _registry.add_component(nameInputEntity, UI::UIComponent(nameInput));

}
