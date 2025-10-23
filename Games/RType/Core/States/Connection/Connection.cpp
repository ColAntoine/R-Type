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

void Connection::update(float delta_time)
{
    if  (!_initialized)
        return;

    _systemLoader.update_all_systems(_registry, delta_time, DLLoader::RenderSystem);
}

void Connection::setup_ui()
{
    std::cout << "[Connection] Setting up UI" << std::endl;

    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    // Create centered panel
    auto connectionPanel = PanelBuilder()
        .centered(renderManager.scalePosY(0))
        .size(renderManager.scaleSizeW(90), renderManager.scaleSizeH(90))
        .backgroundColor(Color{128, 128, 128, 200})
        .border(5, Color{230, 230, 230, 255})
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto panelEntity = _registry.spawn_entity();
    _registry.add_component(panelEntity, UI::UIComponent(connectionPanel));

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
}
