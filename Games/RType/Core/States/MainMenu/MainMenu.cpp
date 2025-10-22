#include "MainMenu.hpp"

void MainMenuState::enter()
{
    std::cout << "[MainMenu] Entering state" << std::endl;

    this->_systemLoader.load_components_from_so("build/lib/libECS.so", this->_registry);
    this->_systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);

    _registry.register_component<RType::UIMainPanel>();
    _registry.register_component<RType::UITitleText>();
    _registry.register_component<RType::UIPlayButton>();
    _registry.register_component<RType::UISettingsButton>();
    _registry.register_component<RType::UIQuitButton>();

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
    if  (!this->_initialized)
        return;

    // this->_systemLoader.update_all_systems(this->_registry, delta_time, DLLoader::LogicSystem);
}

void MainMenuState::on_play_clicked()
{
    std::cout << "[MainMenu] Play button clicked" << std::endl;
    if (this->_stateManager) {
        this->_stateManager->change_state("InGame");
    }
}

void MainMenuState::setup_ui()
{
    std::cout << "[MainMenu] Setting up UI" << std::endl;
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int center_x = sw / 2.0f;
    int center_y = sh / 2.0f;

    auto button_entity = this->_registry.spawn_entity();

    auto componentFactory = this->_systemLoader.get_factory();
    if (componentFactory) {
        componentFactory->create_component<UI::UIButton>(this->_registry, button_entity, center_x - 100, center_y - 25, 200, 50, "PLAY");
    }
    // auto button = std::make_shared<UI::UIButton>(center_x - 100, center_y - 25, 200, 50, "PLAY");
    // UI::ButtonStyle play_style;
    // play_style.setHoveredColor({36, 36, 52, 240});
    // play_style.setNormalColor({20, 20, 30, 220});
    // play_style.setPressedColor({16, 16, 24, 200});
    // play_style.setTextColor({220, 240, 255, 255});
    // play_style.setFontSize(28);
    // button->setStyle(play_style);
    // button->setOnClick([this]() { this->on_play_clicked(); });
    // _registry.add_component(button_entity, UI::UIComponent(button));
}
