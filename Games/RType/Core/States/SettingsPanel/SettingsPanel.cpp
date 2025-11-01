// filepath: Games/RType/Core/States/SettingsPanel/SettingsPanel.cpp
#include "SettingsPanel.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "UI/ThemeManager.hpp"
#include <iostream>

void SettingsPanelState::enter()
{
    std::cout << "[SettingsPanelState] Entering state" << std::endl;

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
    _initialized = true;
}

void SettingsPanelState::exit()
{
    std::cout << "[SettingsPanelState] Exiting state" << std::endl;
    auto &eventBus = MessagingManager::instance().get_event_bus();
    eventBus.unsubscribe(_uiEventCallbackId);
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

    auto &theme = ThemeManager::instance().getTheme();

    auto menuPanel = PanelBuilder()
        .centered(renderManager.scalePosY(0))
        .size(renderManager.scaleSizeW(90), renderManager.scaleSizeH(90))
        .backgroundColor(theme.panelColor)
        .border(5, theme.panelBorderColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto menuPanelEntity = this->_registry.spawn_entity();
    this->_registry.add_component(menuPanelEntity, UI::UIComponent(menuPanel));
}
