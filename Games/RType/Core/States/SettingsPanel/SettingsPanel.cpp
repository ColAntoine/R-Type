// filepath: Games/RType/Core/States/SettingsPanel/SettingsPanel.cpp
#include "SettingsPanel.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include <iostream>

void SettingsPanelState::enter()
{
    std::cout << "[SettingsPanelState] Entering state" << std::endl;

    _systemLoader.load_components_from_so("build/lib/libECS.so", _registry);
    _systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);

    setup_ui();
    _initialized = true;
}

void SettingsPanelState::exit()
{
    std::cout << "[SettingsPanelState] Exiting state" << std::endl;
    _initialized = false;
}

void SettingsPanelState::pause()
{
    std::cout << "[SettingsPanelState] Pausing state" << std::endl;
}

void SettingsPanelState::resume()
{
    std::cout << "[SettingsPanelState] Resuming state" << std::endl;
}

void SettingsPanelState::setup_ui()
{
    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    auto menuPanel = PanelBuilder()
        .centered(renderManager.scalePosY(0))
        .size(renderManager.scaleSizeW(90), renderManager.scaleSizeH(90))
        .backgroundColor(Color{75, 174, 204, 200})
        .border(5, Color{230, 230, 230, 255})
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto menuPanelEntity = this->_registry.spawn_entity();
    this->_registry.add_component(menuPanelEntity, UI::UIComponent(menuPanel));
}
