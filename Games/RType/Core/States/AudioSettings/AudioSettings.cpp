// filepath: Games/RType/Core/States/AudioSettings/AudioSettings.cpp
#include "AudioSettings.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/UIBuilder.hpp"
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

    auto backButton = ButtonBuilder()
        .at(renderManager.scalePosX(11), renderManager.scalePosY(80))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("BACK TO SETTINGS")
        .red()
        .textColor(WHITE)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, WHITE)
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

