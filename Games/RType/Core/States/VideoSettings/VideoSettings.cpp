// filepath: Games/RType/Core/States/VideoSettings/VideoSettings.cpp
#include "VideoSettings.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "UI/Components/GlitchButton.hpp"
#include "UI/ThemeManager.hpp"

#include <iostream>
#include <algorithm>

void VideoSettingsState::enter()
{
    std::cout << "[VideoSettingsState] Entering state" << std::endl;

    #ifdef _WIN32
        const std::string ecsLib = "build/lib/libECS.dll";
        const std::string uiSys = "build/lib/systems/librender_UISystem.dll";
    #else
        const std::string ecsLib = "build/lib/libECS.so";
        const std::string uiSys = "build/lib/systems/librender_UISystem.so";
    #endif

    _systemLoader->load_components(ecsLib, _registry);
    _systemLoader->load_system(uiSys, ILoader::RenderSystem);

    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    filter_available_resolutions(renderManager.get_monitor_width(), renderManager.get_monitor_height());
    set_current_resolution_index(winInfos.getWidth(), winInfos.getHeight());
    get_color_mode_from_theme();
    setup_ui();
    subscribe_to_ui_event();
    _initialized = true;
}

void VideoSettingsState::filter_available_resolutions(int monitorWidth, int monitorHeight)
{
    _availableResolution.erase(
        std::remove_if(_availableResolution.begin(), _availableResolution.end(),
                       [monitorWidth, monitorHeight](const Resolution& res) {
                           return res.width > monitorWidth || res.height > monitorHeight;
                       }),
        _availableResolution.end()
    );
}

void VideoSettingsState::set_current_resolution_index(int currentWidth, int currentHeight)
{
    Resolution currentRes = {currentWidth, currentHeight};

    auto it = std::find_if(_availableResolution.begin(), _availableResolution.end(),
                           [currentRes](const Resolution& res) {
                               return res.width == currentRes.width && res.height == currentRes.height;
                           });
    if (it != _availableResolution.end()) {
        _resolutionIndex = std::distance(_availableResolution.begin(), it);
    } else {
        _availableResolution.push_back(currentRes);
        _resolutionIndex = _availableResolution.size() - 1;
    }
}

void VideoSettingsState::get_color_mode_from_theme()
{
    ColorBlindMode mode = ThemeManager::instance().getColorBlindMode();
    for (size_t i = 0; i < _availableColorModes.size(); ++i) {
        if (std::get<2>(_availableColorModes[i]) == mode) {
            _colorModeIndex = i;
            break;
        }
    }
    if (_colorModeIndex == 0 || _colorModeIndex >= _availableColorModes.size()) {
        _colorModeIndex = 0;
    }
}

void VideoSettingsState::exit()
{
    std::cout << "[VideoSettingsState] Exiting state" << std::endl;
    auto &eventBus = MessagingManager::instance().get_event_bus();
    eventBus.unsubscribe(_uiEventCallbackId);
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

void VideoSettingsState::applyResChange(MoveDirection direction)
{
    auto &renderManager = RenderManager::instance();

    if (direction == MoveDirection::Left) {
        if (_resolutionIndex == 0) {
            _resolutionIndex = _availableResolution.size() - 1;
        } else {
            _resolutionIndex -= 1;
        }
    } else {
        if (_resolutionIndex >= _availableResolution.size() - 1)
            _resolutionIndex = 0;
        else
            _resolutionIndex += 1;
    }
    renderManager.set_window_size(
        _availableResolution[_resolutionIndex].width,
        _availableResolution[_resolutionIndex].height
    );

    auto& eventBus = MessagingManager::instance().get_event_bus();
    Event themeEvent(EventTypes::SCREEN_PARAMETERS_CHANGED);
    eventBus.emit(themeEvent);
}

void VideoSettingsState::applyColorChange(MoveDirection direction)
{
    auto &theme = ThemeManager::instance();

    if (direction == MoveDirection::Left) {
        if (_colorModeIndex == 0) {
            _colorModeIndex = _availableColorModes.size() - 1;
        } else {
            _colorModeIndex -= 1;
        }
    } else {
        if (_colorModeIndex >= _availableColorModes.size() - 1)
            _colorModeIndex = 0;
        else
            _colorModeIndex += 1;
    }

    const auto& [name, palette, colorBlindMode] = _availableColorModes[_colorModeIndex];
    theme.setTheme(palette, colorBlindMode);

    auto& eventBus = MessagingManager::instance().get_event_bus();
    Event themeEvent(EventTypes::SCREEN_PARAMETERS_CHANGED);
    themeEvent.set("palette", palette);
    eventBus.emit(themeEvent);
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
        .text(resToString(_availableResolution[_resolutionIndex]))
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
            this->applyResChange(MoveDirection::Left);
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
            this->applyResChange(MoveDirection::Right);
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
        .text(std::get<0>(_availableColorModes[_colorModeIndex]))
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
            this->applyColorChange(MoveDirection::Left);
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
            this->applyColorChange(MoveDirection::Right);
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto colorModeRightButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(colorModeRightButtonEntity, UI::UIComponent(colorModeRightButton));
}

std::string VideoSettingsState::resToString(const Resolution& res) const
{
    return std::to_string(res.width) + " x " + std::to_string(res.height);
}
