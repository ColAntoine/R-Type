// filepath: Games/RType/Core/States/BindsSettings/BindsSettings.cpp
#include "BindsSettings.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include <iostream>
#include "UI/Components/GlitchButton.hpp"
#include "UI/ThemeManager.hpp"

#include "Core/KeyBindingManager/KeyBindingManager.hpp"
#include "Core/Config/Config.hpp"

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

std::string BindsSettingsState::getTextFromButton(entity buttonEntity)
{
    auto &uiComponent = _registry.get_component<UI::UIComponent>(buttonEntity);
    if (auto *button = dynamic_cast<RType::GlitchButton*>(uiComponent._ui_element.get())) {
        return button->getText();
    }
    return "";
}

void BindsSettingsState::setTextToButton(entity buttonEntity, const std::string& text)
{
    auto &uiComp = this->_registry.get_component<UI::UIComponent>(buttonEntity);
    if (auto* button = dynamic_cast<RType::GlitchButton*>(uiComp._ui_element.get())) {
        button->setText(text);
    }
}

void BindsSettingsState::applyBinding(entity buttonEntity, const std::string &action, CURRENT_BIND toBind)
{
    auto &eventBus = MessagingManager::instance().get_event_bus();
    // Another event to prevent the user from modifying two keys at the same time
    _mouseButtonCallbackId = eventBus.subscribe(EventTypes::MOUSE_PRESSED, [this, buttonEntity](__attribute_maybe_unused__ const Event &event) {
        setTextToButton(buttonEntity, _currentText);
        _currentText = "";

        MessagingManager::instance().get_event_bus().unsubscribe(_currentCallbackId);
        _currentCallbackId = -1;

        MessagingManager::instance().get_event_bus().unsubscribe_deferred(_mouseButtonCallbackId);
        _mouseButtonCallbackId = -1;
    });

    _currentText = getTextFromButton(buttonEntity);
    setTextToButton(buttonEntity, "");
    _currentCallbackId = eventBus.subscribe(EventTypes::KEY_PRESSED, [this, buttonEntity, action, toBind](const Event& event) {
        std::string keyStr = event.get<std::string>("key");

        setTextToButton(buttonEntity, keyStr);
        MessagingManager::instance().get_event_bus().unsubscribe_deferred(_currentCallbackId);
        MessagingManager::instance().get_event_bus().unsubscribe_deferred(_mouseButtonCallbackId);

        _currentCallbackId = -1;
        _mouseButtonCallbackId = -1;

        auto &keyBinds = KeyBindingManager::instance();
        int keyCode = keyBinds.stringToKeyCode(keyStr);
        keyBinds.setKeyBinding(action, keyCode);

        Config::instance().setValueOf("controls", action, keyStr);
    });
}

void BindsSettingsState::setup_ui()
{
    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    auto &theme = ThemeManager::instance().getTheme();

    auto &keyBindsManager = KeyBindingManager::instance();

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

    // UP BUTTON
    auto upText = TextBuilder()
        .at(renderManager.scalePosX(62), renderManager.scalePosY(15))
        .text("UP")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto upTextEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(upTextEntity, UI::UIComponent(upText));

    _up = this->_registry.spawn_entity();
    auto upBindButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(60), renderManager.scalePosY(22))
        .size(renderManager.scaleSizeW(7), renderManager.scaleSizeH(12))
        .text(keyBindsManager.getValueAssociatedToKeyCode(keyBindsManager.getKeyBinding("move_up")))
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->applyBinding(_up, "move_up", CURRENT_BIND::UP);
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    this->_registry.add_component<UI::UIComponent>(_up, UI::UIComponent(upBindButton));

    // DOWN BUTTON
    auto downText = TextBuilder()
        .at(renderManager.scalePosX(60), renderManager.scalePosY(50))
        .text("DOWN")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto downTextEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(downTextEntity, UI::UIComponent(downText));

    _down = this->_registry.spawn_entity();
    auto downBindButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(60), renderManager.scalePosY(36))
        .size(renderManager.scaleSizeW(7), renderManager.scaleSizeH(12))
        .text(keyBindsManager.getValueAssociatedToKeyCode(keyBindsManager.getKeyBinding("move_down")))
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->applyBinding(_down, "move_down", CURRENT_BIND::DOWN);
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    this->_registry.add_component<UI::UIComponent>(_down, UI::UIComponent(downBindButton));

    // LEFT BUTTON
    auto leftText = TextBuilder()
        .at(renderManager.scalePosX(53), renderManager.scalePosY(29))
        .text("LEFT")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto leftTextEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(leftTextEntity, UI::UIComponent(leftText));

    _left = this->_registry.spawn_entity();
    auto leftBindButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(52), renderManager.scalePosY(36))
        .size(renderManager.scaleSizeW(7), renderManager.scaleSizeH(12))
        .text(keyBindsManager.getValueAssociatedToKeyCode(keyBindsManager.getKeyBinding("move_left")))
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->applyBinding(_left, "move_left", CURRENT_BIND::LEFT);
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    this->_registry.add_component<UI::UIComponent>(_left, UI::UIComponent(leftBindButton));

    // RIGHT BUTTON
    auto rightText = TextBuilder()
        .at(renderManager.scalePosX(69), renderManager.scalePosY(29))
        .text("RIGHT")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto rightTextEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(rightTextEntity, UI::UIComponent(rightText));

    _right = this->_registry.spawn_entity();
    auto rightBindButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(68), renderManager.scalePosY(36))
        .size(renderManager.scaleSizeW(7), renderManager.scaleSizeH(12))
        .text(keyBindsManager.getValueAssociatedToKeyCode(keyBindsManager.getKeyBinding("move_right")))
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->applyBinding(_right, "move_right", CURRENT_BIND::RIGHT);
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    this->_registry.add_component<UI::UIComponent>(_right, UI::UIComponent(rightBindButton));

    // SHOOT BUTTON
    auto shootText = TextBuilder()
        .at(renderManager.scalePosX(60), renderManager.scalePosY(70))
        .text("SHOOT")
        .fontSize(renderManager.scaleSizeW(3))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto shootTextEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(shootTextEntity, UI::UIComponent(shootText));

    _shoot = this->_registry.spawn_entity();
    auto shootBindButton = GlitchButtonBuilder()
        .at(renderManager.scalePosX(52), renderManager.scalePosY(77))
        .size(renderManager.scaleSizeW(23), renderManager.scaleSizeH(11))
        .text(keyBindsManager.getValueAssociatedToKeyCode(keyBindsManager.getKeyBinding("shoot")))
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->applyBinding(_shoot, "shoot", CURRENT_BIND::SHOOT);
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    this->_registry.add_component<UI::UIComponent>(_shoot, UI::UIComponent(shootBindButton));
}
