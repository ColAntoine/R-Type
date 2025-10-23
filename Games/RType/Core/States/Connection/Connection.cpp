#include "Connection.hpp"

#include <string>
#include "Constants.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"

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
}
