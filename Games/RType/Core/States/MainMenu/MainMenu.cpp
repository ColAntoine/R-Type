#include "MainMenu.hpp"
#include "Constants.hpp"
#include "ECS/UI/Components/Button.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"

void MainMenuState::enter()
{
    std::cout << "[MainMenu] Entering state" << std::endl;

    this->_systemLoader.load_components_from_so("build/lib/libECS.so", this->_registry);
    this->_systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);

    this->_registry.register_component<UI::UIButton>();
    // _registry.register_component<RType::UIMainPanel>();
    // _registry.register_component<RType::UITitleText>();
    // _registry.register_component<RType::UISettingsButton>();
    // _registry.register_component<RType::UIQuitButton>();

    this->setup_ui();
    this->_initialized = true;
}

void MainMenuState::exit()
{
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
        this->_stateManager->push_state("InGame");
        this->_stateManager->push_state("InGameHud");
    }
}

void MainMenuState::play_coop()
{
    if (this->_stateManager) {
        this->_stateManager->pop_state();
        this->_stateManager->push_state("Connection");
    }
}

void MainMenuState::play_settings()
{
    if (this->_stateManager) {
        this->_stateManager->pop_state();
        this->_stateManager->push_state("Settings");
    }
}

void MainMenuState::play_quit()
{
    if (this->_stateManager) {
        this->_stateManager->clear_states();
    }
}

void MainMenuState::setup_ui()
{
    std::cout << "[MainMenu] Setting up UI" << std::endl;

    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();

    auto menuPanel = PanelBuilder()
        .centered(renderManager.scalePosY(0))
        .size(renderManager.scaleSizeW(60), renderManager.scaleSizeH(80))
        .backgroundColor(Color{75, 174, 204, 200})
        .border(5, Color{230, 230, 230, 255})
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto menuPanelEntity = this->_registry.spawn_entity();
    this->_registry.add_component(menuPanelEntity, UI::UIComponent(menuPanel));

    auto title = TextBuilder()
        .centered(renderManager.scalePosY(-20))
        .text("R - TYPE")
        .fontSize(renderManager.scaleSizeW(12))
        .textColor(WHITE)
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto titleEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(titleEntity, UI::UIComponent(title));

    auto soloButton = ButtonBuilder()
        .centered(renderManager.scalePosY(0))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("SOLO MODE")
        .color(Color{75, 174, 204, 255})
        .textColor(WHITE)
        .fontSize(24)
        .border(2, WHITE)
        .onClick([this]() {
            this->play_solo();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto soloButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(soloButtonEntity, UI::UIComponent(soloButton));

    auto coopButton = ButtonBuilder()
        .centered(renderManager.scalePosY(10))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("COOP MODE")
        .color(Color{75, 174, 204, 255})
        .textColor(WHITE)
        .fontSize(24)
        .border(2, WHITE)
        .onClick([this]() {
            this->play_coop();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto coopButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(coopButtonEntity, UI::UIComponent(coopButton));

    auto settingsButton = ButtonBuilder()
        .centered(renderManager.scalePosY(20))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("SETTINGS")
        .color(Color{75, 174, 204, 255})
        .textColor(WHITE)
        .fontSize(24)
        .border(2, WHITE)
        .onClick([this]() {
            this->play_settings();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto settingsButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(settingsButtonEntity, UI::UIComponent(settingsButton));

    auto quitButton = ButtonBuilder()
        .centered(renderManager.scalePosY(30))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("QUIT GAME")
        .red()
        .textColor(WHITE)
        .fontSize(24)
        .border(2, WHITE)
        .onClick([this]() {
            this->play_quit();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto quitButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component<UI::UIComponent>(quitButtonEntity, UI::UIComponent(quitButton));
}
