#include "Settings.hpp"
#include "ECS/UI/Components/Text.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "UI/Components/GlitchButton.hpp"
#include "UI/ThemeManager.hpp"
#include <iostream>
#include <random>
#include <cmath>

void SettingsState::enter()
{
    std::cout << "[Settings] Entering state" << std::endl;

    #ifdef _WIN32
        const std::string ecsLib = "build/lib/libECS.dll";
        const std::string uiSys = "build/lib/systems/librender_UISystem.dll";
    #else
        const std::string ecsLib = "build/lib/libECS.so";
        const std::string uiSys = "build/lib/systems/librender_UISystem.so";
    #endif

    this->_systemLoader->load_components(ecsLib, _registry);
    this->_systemLoader->load_system(uiSys, ILoader::RenderSystem);

    this->_registry.register_component<UI::UIButton>();
    this->_registry.register_component<UI::UIText>();

    setup_ui();
    subscribe_to_ui_event();
    this->_initialized = true;
}

void SettingsState::exit()
{
    std::cout << "[Menus Background] Exiting state" << std::endl;
    auto &eventBus = MessagingManager::instance().get_event_bus();
    eventBus.unsubscribe(_uiEventCallbackId);
    this->_initialized = false;
}

void SettingsState::pause()
{
    std::cout << "[Menus Background] Pausing state" << std::endl;
}

void SettingsState::resume()
{
    std::cout << "[Menus Background] Resuming state" << std::endl;
}

void SettingsState::play_back_menu()
{
    if (this->_stateManager) {
        this->_stateManager->pop_state();
        this->_stateManager->pop_state();
        this->_stateManager->push_state("MainMenu");
    }
}

void SettingsState::play_bind_menu()
{
    if (this->_stateManager) {
        this->_stateManager->pop_state();
        this->_stateManager->push_state("BindsSettings");
    }
}

void SettingsState::play_audio_menu()
{
    if (this->_stateManager) {
        this->_stateManager->pop_state();
        this->_stateManager->push_state("AudioSettings");
    }
}

void SettingsState::play_video_menu()
{
    if (this->_stateManager) {
        this->_stateManager->pop_state();
        this->_stateManager->push_state("VideoSettings");
    }
}

void SettingsState::play_credit_menu()
{
    if (this->_stateManager) {
        this->_stateManager->pop_state();
        this->_stateManager->push_state("Credits");
    }
}

void SettingsState::setup_ui()
{
    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    auto &theme = ThemeManager::instance().getTheme();

    auto title = TextBuilder()
        .centered(renderManager.scalePosY(-28))
        .text("R - TYPE")
        .fontSize(renderManager.scaleSizeW(12))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto titleEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(titleEntity, UI::UIComponent(title));

    auto subTitle = TextBuilder()
        .centered(renderManager.scalePosY(-15))
        .text("SETTINGS")
        .fontSize(renderManager.scaleSizeW(5))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto subTitleEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(subTitleEntity, UI::UIComponent(subTitle));

    auto backButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(11), renderManager.scalePosY(80))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("BACK TO THE MENU")
        .color(theme.exitButtonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.exitButtonColors.border)
        .neonColors(theme.exitButtonColors.neonColor, theme.exitButtonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->play_back_menu();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto backButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(backButtonEntity, UI::UIComponent(backButton));

    auto creditsButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(89) - renderManager.scaleSizeW(20), renderManager.scalePosY(80))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("CREDITS")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->play_credit_menu();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto creditsButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(creditsButtonEntity, UI::UIComponent(creditsButton));

    auto audioButton = GlitchButtonBuilder()
        .centered(renderManager.scalePosY(0))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("AUDIO SETTINGS")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->play_audio_menu();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto audioButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(audioButtonEntity, UI::UIComponent(audioButton));

    auto videoButton = GlitchButtonBuilder()
        .centered(renderManager.scalePosY(10))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("VIDEO SETTINGS")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->play_video_menu();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto videoButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(videoButtonEntity, UI::UIComponent(videoButton));

    auto bindsButton = GlitchButtonBuilder()
        .centered(renderManager.scalePosY(20))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("BINDINGS")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->play_bind_menu();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto bindsButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(bindsButtonEntity, UI::UIComponent(bindsButton));
}
