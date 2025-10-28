// filepath: Games/RType/Core/States/BindsSettings/BindsSettings.cpp
#include "BindsSettings.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include <iostream>
#include "UI/Components/GlitchButton.hpp"
#include "UI/ThemeManager.hpp"

void BindsSettingsState::enter()
{
    std::cout << "[BindsSettingsState] Entering state" << std::endl;

    _systemLoader.load_components_from_so("build/lib/libECS.so", _registry);
    _systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);

    setup_ui();
    subscribe_to_ui_event();
    _initialized = true;
}

void BindsSettingsState::exit()
{
    std::cout << "[BindsSettingsState] Exiting state" << std::endl;
    auto &eventBus = MessagingManager::instance().get_event_bus();
    eventBus.unsubscribe(_uiEventCallbackId);
    _initialized = false;
}

void BindsSettingsState::pause()
{
    std::cout << "[BindsSettingsState] Pausing state" << std::endl;
}

void BindsSettingsState::resume()
{
    std::cout << "[BindsSettingsState] Resuming state" << std::endl;
}

void BindsSettingsState::setup_ui()
{
    std::cout << "[BindsSettingsState] Setting up UI" << std::endl;
    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    auto &theme = ThemeManager::instance().getTheme();

    auto backButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(11), renderManager.scalePosY(80))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("BACK TO SETTINGS")
        .color(theme.exitButtonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.exitButtonColors.border)
        .neonColors(theme.exitButtonColors.neonColor, theme.exitButtonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            if (this->_stateManager) {
                this->_stateManager->pop_state();
                this->_stateManager->push_state("Settings");
            }
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto backButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(backButtonEntity, UI::UIComponent(backButton));
}
