#include "Settings.hpp"
#include "ECS/UI/Components/Text.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include <iostream>
#include <random>
#include <cmath>

void SettingsState::enter()
{
    std::cout << "[Settings] Entering state" << std::endl;

    this->_systemLoader.load_components_from_so("build/lib/libECS.so", this->_registry);
    this->_systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);

    this->_registry.register_component<UI::UIButton>();
    this->_registry.register_component<UI::UIText>();

    setup_ui();
    this->_initialized = true;
}

void SettingsState::exit()
{
    std::cout << "[Menus Background] Exiting state" << std::endl;
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

void SettingsState::update(float delta_time)
{
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
}

void SettingsState::play_audio_menu()
{
}

void SettingsState::play_video_menu()
{
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

    auto backButton = ButtonBuilder()
        .at(renderManager.scalePosX(11), renderManager.scalePosY(80))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("BACK TO THE MENU")
        .red()
        .textColor(WHITE)
        .fontSize(24)
        .border(2, WHITE)
        .onClick([this]() {
            this->play_back_menu();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto backButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(backButtonEntity, UI::UIComponent(backButton));

    auto creditsButton = ButtonBuilder()
        .centered(renderManager.scalePosY(0))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("CREDITS")
        .color(Color{75, 174, 204, 255})
        .textColor(WHITE)
        .fontSize(24)
        .border(2, WHITE)
        .onClick([this]() {
            this->play_credit_menu();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto creditsButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(creditsButtonEntity, UI::UIComponent(creditsButton));
}
