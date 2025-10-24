#include "MainMenu.hpp"
#include "Constants.hpp"
#include "ECS/UI/Components/Button.hpp"
#include "ECS/UI/UIBuilder.hpp"

void MainMenuState::enter()
{
    std::cout << "[MainMenu] Entering state" << std::endl;

    this->_systemLoader.load_components_from_so("build/lib/libECS.so", this->_registry);
    this->_systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);

    this->_registry.register_component<UI::UIButton>();
    // _registry.register_component<RType::UIMainPanel>();
    // _registry.register_component<RType::UITitleText>();
    _registry.register_component<RType::UIPlayButton>();
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

void MainMenuState::update(__attribute_maybe_unused__ float delta_time)
{
    if  (!this->_initialized)
        return;

    // this->_systemLoader.update_all_systems(this->_registry, delta_time, DLLoader::LogicSystem);
}

void MainMenuState::on_play_clicked()
{
    std::cout << "[MainMenu] Play button clicked" << std::endl;
    if (this->_stateManager) {

        this->_stateManager->pop_state();
        this->_stateManager->push_state("InGame");
        this->_stateManager->push_state("InGameHud");
        this->_stateManager->push_state("InGameBackground");
    }
}

void MainMenuState::setup_ui()
{
    std::cout << "[MainMenu] Setting up UI" << std::endl;

    auto playButton = ButtonBuilder()
        .centered(200)
        .size(400, 100)
        .text("PLAY")
        .blue()
        .textColor(WHITE)
        .fontSize(24)
        .border(2, WHITE)
        .onClick([this]() {
            this->on_play_clicked();
        }
    )
    .build(SCREEN_WIDTH, SCREEN_HEIGHT);
    auto playButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component(playButtonEntity, UI::UIComponent(playButton));

}
