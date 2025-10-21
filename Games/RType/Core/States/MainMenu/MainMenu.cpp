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

    this->_uiSystems->update(*this->_uiRegistry, delta_time);
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

    auto button_entity = _uiRegistry->spawn_entity();
    auto button = std::make_shared<UI::UIButton>(center_x - 100, center_y - 25, 200, 50, "PLAY");
    UI::ButtonStyle play_style;
    play_style._normal_color = {20, 20, 30, 220};
    play_style._hovered_color = {36, 36, 52, 240};
    play_style._pressed_color = {16, 16, 24, 200};
    play_style._text_color = {220, 240, 255, 255};
    play_style._font_size = 28;
    button->set_style(play_style);
    button->set_on_click([this]() { this->on_play_clicked(); });
    _uiRegistry->add_component(button_entity, UI::UIComponent(button));
}
