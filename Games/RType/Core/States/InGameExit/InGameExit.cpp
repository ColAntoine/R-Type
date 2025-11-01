#include "InGameExit.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/Messaging/MessagingManager.hpp"
#include "UI/Components/GlitchButton.hpp"
#include "UI/ThemeManager.hpp"
#include <iostream>

void InGameExitState::enter()
{
    std::cout << "[InGameExit] Entering state" << std::endl;
    _systemLoader->load_components("build/lib/libECS" + ext, _registry);
    _systemLoader->load_system("build/lib/systems/librender_UISystem" + ext, ILoader::RenderSystem);

    this->_registry.register_component<UI::UIButton>();

    setup_ui();
    subscribe_to_ui_event();
    _initialized = true;
}

void InGameExitState::exit()
{
    auto &eventBus = MessagingManager::instance().get_event_bus();
    eventBus.unsubscribe(_uiEventCallbackId);
    _initialized = false;
}

void InGameExitState::pause()
{
}

void InGameExitState::resume()
{
}

void InGameExitState::setup_ui()
{
    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();
    auto &theme = ThemeManager::instance().getTheme();

    float centerX = winInfos.getWidth() / 2.0f;
    float centerY = winInfos.getHeight() / 2.0f;

    // Create styled dialog panel (like SettingsPanel)
    auto dialogPanel = PanelBuilder()
        .centered(renderManager.scalePosY(0))
        .size(renderManager.scaleSizeW(60), renderManager.scaleSizeH(50))
        .backgroundColor(theme.panelColor)
        .border(5, theme.panelBorderColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto dialogEnt = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(dialogEnt, UI::UIComponent(dialogPanel));

    // Title text
    auto titleText = TextBuilder()
        .centered(renderManager.scalePosY(-15))
        .text("Do you really want to quit?")
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(3))
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto titleEnt = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(titleEnt, UI::UIComponent(titleText));

    // YES Button
    auto yesButton = GlitchButtonBuilder()
        .at(centerX - renderManager.scaleSizeW(20), centerY + renderManager.scaleSizeH(5))
        .size(renderManager.scaleSizeW(15), renderManager.scaleSizeH(8))
        .text("YES")
        .color(theme.exitButtonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2.5))
        .border(2, theme.exitButtonColors.border)
        .neonColors(theme.exitButtonColors.neonColor, theme.exitButtonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            std::cout << "[InGameExit] YES button pressed - exiting to menu" << std::endl;
            if (_stateManager) {
                _stateManager->pop_state();  // Remove InGameExit
                _stateManager->pop_state();  // Remove InGameHud
                _stateManager->pop_state();  // Remove InGame
                _stateManager->pop_state();  // Remove InGameBackground
                _stateManager->push_state("MenusBG", false);
                _stateManager->push_state("MainMenu", false);
            }
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto yesEnt = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(yesEnt, UI::UIComponent(yesButton));

    // NO Button
    auto noButton = GlitchButtonBuilder()
        .at(centerX + renderManager.scaleSizeW(5), centerY + renderManager.scaleSizeH(5))
        .size(renderManager.scaleSizeW(15), renderManager.scaleSizeH(8))
        .text("NO")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2.5))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            std::cout << "[InGameExit] NO button pressed - back to game" << std::endl;
            if (_stateManager) {
                _stateManager->pop_state();  // Remove InGameExit, back to InGame
            }
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto noEnt = _registry.spawn_entity();
    _registry.add_component<UI::UIComponent>(noEnt, UI::UIComponent(noButton));
}

void InGameExitState::update(float delta_time)
{
    if (!_initialized)
        return;

    _systemLoader->update_all_systems(_registry, delta_time, ILoader::LogicSystem);
}

