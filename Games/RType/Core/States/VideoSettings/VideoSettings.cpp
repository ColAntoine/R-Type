// filepath: Games/RType/Core/States/VideoSettings/VideoSettings.cpp
#include "VideoSettings.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include <iostream>
#include "UI/Components/GlitchButton.hpp"
#include "UI/ThemeManager.hpp"

void VideoSettingsState::enter()
{
    std::cout << "[VideoSettingsState] Entering state" << std::endl;

    _systemLoader.load_components_from_so("build/lib/libECS.so", _registry);
    _systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);

    _currentResIndex = 3; // Default to 1920x1080
    _nextResIndex = _currentResIndex;
    _currentColorModeIndex = 0;
    _nextColorModeIndex = _currentColorModeIndex;

    setup_ui();
    _initialized = true;
}

void VideoSettingsState::exit()
{
    std::cout << "[VideoSettingsState] Exiting state" << std::endl;
    _initialized = false;
}

void VideoSettingsState::pause()
{
    std::cout << "[VideoSettingsState] Pausing state" << std::endl;
}

void VideoSettingsState::resume()
{
    std::cout << "[VideoSettingsState] Resuming state" << std::endl;
}

void VideoSettingsState::apply_resolution_change(MoveDirection direction)
{
    if (direction == MoveDirection::Left) {
        if (_nextResIndex > 0)
            _nextResIndex--;
        else
            _nextResIndex = _availableResolutions.size() - 1;
    } else if (direction == MoveDirection::Right) {
        if (_nextResIndex < static_cast<int>(_availableResolutions.size()) - 1)
            _nextResIndex++;
        else
            _nextResIndex = 0;
    }

    if (_currentResIndex != _nextResIndex) {
        auto &renderManager = RenderManager::instance();
        const auto& newRes = _availableResolutions[_nextResIndex];
        renderManager.set_window_size(newRes.width, newRes.height);

        _currentResIndex = _nextResIndex;
    }
}

void VideoSettingsState::setup_ui()
{
    std::cout << "[VideoSettingsState] Setting up UI" << std::endl;
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

    // RESOLUTION SETTINGS
    auto resolution = TextBuilder()
        .centered(renderManager.scalePosY(-27))
        .text("Resolution")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto resolutionEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(resolutionEntity, UI::UIComponent(resolution));

    auto currentResolution = TextBuilder()
        .centered(renderManager.scalePosY(-16))
        .text(resToString(_availableResolutions[_currentResIndex]))
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(theme.secondaryTextColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto currentResolutionEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(currentResolutionEntity, UI::UIComponent(currentResolution));

    auto resLeftButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(30), renderManager.scalePosY(28))
        .size(renderManager.scaleSizeW(6), renderManager.scaleSizeH(11))
        .text("<")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->apply_resolution_change(MoveDirection::Left);
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto resLeftButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(resLeftButtonEntity, UI::UIComponent(resLeftButton));

    auto resRightButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(70) - renderManager.scaleSizeW(6), renderManager.scalePosY(28))
        .size(renderManager.scaleSizeW(6), renderManager.scaleSizeH(11))
        .text(">")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->apply_resolution_change(MoveDirection::Right);
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto resRightButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(resRightButtonEntity, UI::UIComponent(resRightButton));

    // COLOR SETTINGS
    auto colorMode = TextBuilder()
        .centered(renderManager.scalePosY(-5))
        .text("Color Mode")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto currentColorMode = TextBuilder()
        .centered(renderManager.scalePosY(6))
        .text(_availableColorModes[_currentColorModeIndex])
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(theme.secondaryTextColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto currentColorModeEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(currentColorModeEntity, UI::UIComponent(currentColorMode));

    auto colorModeEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(colorModeEntity, UI::UIComponent(colorMode));

    auto colorModeLeftButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(30), renderManager.scalePosY(50))
        .size(renderManager.scaleSizeW(6), renderManager.scaleSizeH(11))
        .text("<")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            std::cout << "Changed color mode" << std::endl;
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto colorModeLeftButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(colorModeLeftButtonEntity, UI::UIComponent(colorModeLeftButton));

    auto colorModeRightButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(70) - renderManager.scaleSizeW(6), renderManager.scalePosY(50))
        .size(renderManager.scaleSizeW(6), renderManager.scaleSizeH(11))
        .text(">")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            std::cout << "Changed color mode" << std::endl;
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto colorModeRightButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(colorModeRightButtonEntity, UI::UIComponent(colorModeRightButton));
}

std::string VideoSettingsState::resToString(const Resolution& res) const
{
    return std::to_string(res.width) + " x " + std::to_string(res.height);
}
