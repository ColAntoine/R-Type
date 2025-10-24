// filepath: Games/RType/Core/States/AudioSettings/AudioSettings.cpp
#include "AudioSettings.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "UI/Components/GlitchButton.hpp"
#include "UI/ThemeManager.hpp"
#include <iostream>

void AudioSettingsState::enter()
{
    std::cout << "[AudioSettingsState] Entering state" << std::endl;

    _systemLoader.load_components_from_so("build/lib/libECS.so", _registry);
    _systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);

    setup_ui();
    _initialized = true;
}

void AudioSettingsState::exit()
{
    std::cout << "[AudioSettingsState] Exiting state" << std::endl;
    _initialized = false;
}

void AudioSettingsState::pause()
{
    std::cout << "[AudioSettingsState] Pausing state" << std::endl;
}

void AudioSettingsState::resume()
{
    std::cout << "[AudioSettingsState] Resuming state" << std::endl;
}

void AudioSettingsState::setup_ui()
{
    std::cout << "[AudioSettingsState] Setting up UI" << std::endl;

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

    // GENERAL SOUND SETTINGS
    auto general = TextBuilder()
        .centered(renderManager.scalePosY(-37))
        .text("General volume")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto generalEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(generalEntity, UI::UIComponent(general));

    auto downGeneralButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(30), renderManager.scalePosY(18))
        .size(renderManager.scaleSizeW(6), renderManager.scaleSizeH(11))
        .text("-")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            std::cout << "Decrease general volume button clicked" << std::endl;
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto downGeneralButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(downGeneralButtonEntity, UI::UIComponent(downGeneralButton));

    auto upGeneralButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(70) - renderManager.scaleSizeW(6), renderManager.scalePosY(18))
        .size(renderManager.scaleSizeW(6), renderManager.scaleSizeH(11))
        .text("+")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            std::cout << "Increase general volume button clicked" << std::endl;
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto upGeneralButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(upGeneralButtonEntity, UI::UIComponent(upGeneralButton));

    // MUSIC SOUND SETTINGS
    auto music = TextBuilder()
        .centered(renderManager.scalePosY(-15))
        .text("Music volume")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto musicEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(musicEntity, UI::UIComponent(music));

    auto downMusicButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(30), renderManager.scalePosY(40))
        .size(renderManager.scaleSizeW(6), renderManager.scaleSizeH(11))
        .text("-")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            std::cout << "Decrease music button clicked" << std::endl;
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto downMusicButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(downMusicButtonEntity, UI::UIComponent(downMusicButton));

    auto upMusicButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(70) - renderManager.scaleSizeW(6), renderManager.scalePosY(40))
        .size(renderManager.scaleSizeW(6), renderManager.scaleSizeH(11))
        .text("+")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            std::cout << "Increase music button clicked" << std::endl;
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto upMusicButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(upMusicButtonEntity, UI::UIComponent(upMusicButton));

    // SFX SOUND SETTINGS
    auto sfx = TextBuilder()
        .centered(renderManager.scalePosY(7))
        .text("Game sound volume")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto sfxEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(sfxEntity, UI::UIComponent(sfx));

    auto downSfxButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(30), renderManager.scalePosY(62))
        .size(renderManager.scaleSizeW(6), renderManager.scaleSizeH(11))
        .text("-")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            std::cout << "Decrease sfx button clicked" << std::endl;
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto downSfxButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(downSfxButtonEntity, UI::UIComponent(downSfxButton));

    auto upSfxButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(70) - renderManager.scaleSizeW(6), renderManager.scalePosY(62))
        .size(renderManager.scaleSizeW(6), renderManager.scaleSizeH(11))
        .text("+")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            std::cout << "Increase sfx button clicked" << std::endl;
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto upSfxButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(upSfxButtonEntity, UI::UIComponent(upSfxButton));
}

