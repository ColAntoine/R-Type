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
    std::cout << "[MainMenu] Exiting state" << std::endl;
    this->cleanup_ui();
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

void MainMenuState::update(float delta_time)
{
    // if  (!this->_initialized)
    //     return;
    // this->_systemLoader.update_all_systems(this->_registry, delta_time, DLLoader::LogicSystem);
}

void MainMenuState::on_play_clicked()
{
    if (this->_stateManager) {
        this->_stateManager->change_state("InGame");
    }
}

void MainMenuState::on_settings_clicked()
{
    // if (this->_stateManager) {
    // }
}

void MainMenuState::on_quit_clicked()
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
    this->_registry.add_component(titleEntity, UI::UIComponent(title));

    auto playButton = ButtonBuilder()
        .centered(renderManager.scalePosY(2))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("PLAY")
        .color(Color{75, 174, 204, 255})
        .textColor(WHITE)
        .fontSize(24)
        .border(2, WHITE)
        .onClick([this]() {
            this->on_play_clicked();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto playButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component(playButtonEntity, UI::UIComponent(playButton));

    auto settingsButton = ButtonBuilder()
        .centered(renderManager.scalePosY(12))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("SETTINGS")
        .color(Color{75, 174, 204, 255})
        .textColor(WHITE)
        .fontSize(24)
        .border(2, WHITE)
        .onClick([this]() {
            this->on_settings_clicked();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto settingsButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component(settingsButtonEntity, UI::UIComponent(settingsButton));

    auto quitButton = ButtonBuilder()
        .centered(renderManager.scalePosY(22))
        .size(renderManager.scaleSizeW(20), renderManager.scaleSizeH(8))
        .text("QUIT GAME")
        .red()
        .textColor(WHITE)
        .fontSize(24)
        .border(2, WHITE)
        .onClick([this]() {
            this->on_quit_clicked();
        })
        .build(winInfos.getWidth(), winInfos.getHeight());

    auto quitButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component(quitButtonEntity, UI::UIComponent(quitButton));
}
