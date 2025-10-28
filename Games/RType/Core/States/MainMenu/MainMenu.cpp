#include "MainMenu.hpp"
#include "Constants.hpp"
#include "ECS/UI/Components/Button.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "UI/Components/GlitchButton.hpp"
#include "UI/ThemeManager.hpp"

void MainMenuState::enter()
{
    std::cout << "[MainMenu] Entering state" << std::endl;

    this->_systemLoader.load_components_from_so("build/lib/libECS.so", this->_registry);
    this->_systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);

    this->_registry.register_component<UI::UIButton>();

    this->setup_ui();
    subscribe_to_ui_event();
    this->_initialized = true;
}

void MainMenuState::exit()
{
    auto &eventBus = MessagingManager::instance().get_event_bus();
    eventBus.unsubscribe(_uiEventCallbackId);
    this->_initialized = false;
}

void MainMenuState::pause()
{
    std::cout << "[MainMenu] Pausing state" << std::endl;
}

void MainMenuState::resume()
{
    std::cout << "[MainMenu] Resuming state" << std::endl;
}

void MainMenuState::update(__attribute_maybe_unused__ float delta_time)
{
}

void MainMenuState::play_solo()
{

    if (this->_stateManager) {

        this->_stateManager->pop_state();
        this->_stateManager->push_state("InGame");
        this->_stateManager->push_state("InGameHud");
        this->_stateManager->push_state("InGameBackground");
    }
}

void MainMenuState::play_coop()
{
    if (this->_stateManager) {
        this->_stateManager->pop_state();
        this->_stateManager->push_state("SettingsPanel");
        this->_stateManager->push_state("Connection");
    }
}

void MainMenuState::play_quit()
{
    if (this->_stateManager) {
        this->_stateManager->clear_states();
    }
}

void MainMenuState::play_settings()
{
    if (this->_stateManager) {
        this->_stateManager->pop_state();
        this->_stateManager->push_state("SettingsPanel");
        this->_stateManager->push_state("Settings");
    }
}

void MainMenuState::setup_ui()
{
    std::cout << "[MainMenu] Setting up UI" << std::endl;

    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    auto &theme = ThemeManager::instance().getTheme();

    auto menuPanel = PanelBuilder()
        .centered(renderManager.scalePosY(0))
        .size(renderManager.scaleSizeW(60), renderManager.scaleSizeH(80))
        .backgroundColor(theme.panelColor)
        .border(5, theme.panelBorderColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto menuPanelEntity = this->_registry.spawn_entity();
    this->_registry.add_component(menuPanelEntity, UI::UIComponent(menuPanel));

    auto title = TextBuilder()
        .centered(renderManager.scalePosY(-20))
        .text("R - TYPE")
        .fontSize(renderManager.scaleSizeW(12))
        .textColor(theme.textColor)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto titleEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(titleEntity, UI::UIComponent(title));

    auto soloButton = GlitchButtonBuilder()
        .centered(renderManager.scalePosY(0))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("SOLO MODE")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->play_solo();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto soloButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(soloButtonEntity, UI::UIComponent(soloButton));

    auto coopButton = GlitchButtonBuilder()
        .centered(renderManager.scalePosY(10))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("COOP MODE")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->play_coop();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto coopButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(coopButtonEntity, UI::UIComponent(coopButton));

    auto settingsButton = GlitchButtonBuilder()
        .centered(renderManager.scalePosY(20))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("SETTINGS")
        .color(theme.buttonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.buttonColors.border)
        .neonColors(theme.buttonColors.neonColor, theme.buttonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->play_settings();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto settingsButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(settingsButtonEntity, UI::UIComponent(settingsButton));

    auto quitButton = GlitchButtonBuilder()
        .centered(renderManager.scalePosY(30))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("QUIT GAME")
        .color(theme.exitButtonColors.normal)
        .textColor(theme.textColor)
        .fontSize(renderManager.scaleSizeW(2))
        .border(2, theme.exitButtonColors.border)
        .neonColors(theme.exitButtonColors.neonColor, theme.exitButtonColors.neonGlowColor)
        .glitchParams(2.0f, 8.0f, true)
        .onClick([this]() {
            this->play_quit();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());


    auto quitButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(quitButtonEntity, UI::UIComponent(quitButton));
}
