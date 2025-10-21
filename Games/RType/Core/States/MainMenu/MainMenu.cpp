#include "MainMenu.hpp"

void MainMenuState::enter()
{
    std::cout << "[MainMenu] Entering state" << std::endl;

    _uiRegistry->register_component<UI::UIComponent>();
    _uiRegistry->register_component<RType::UIMainPanel>();
    _uiRegistry->register_component<RType::UITitleText>();
    _uiRegistry->register_component<RType::UIPlayButton>();
    _uiRegistry->register_component<RType::UISettingsButton>();
    _uiRegistry->register_component<RType::UIQuitButton>();
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
    std::cout << "[MainMenu] Updating state with delta_time: " << delta_time << std::endl;
    if  (!this->_initialized)
        return;

    this->_uiSystems->update(*this->_uiRegistry, delta_time);
}

void MainMenuState::render()
{
    std::cout << "[MainMenu] Rendering state" << std::endl;
}
