#include "InGame.hpp"

void InGameState::enter()
{
    std::cout << "[InGame] Entering state" << std::endl;

    this->setup_ui();
    this->_initialized = true;
}

void InGameState::exit()
{
    std::cout << "[InGame] Exiting state" << std::endl;
    this->cleanup_ui();
    this->_initialized = false;
}

void InGameState::pause()
{
    std::cout << "[InGame] Pausing state" << std::endl;
}

void InGameState::resume()
{
    std::cout << "[InGame] Resuming state" << std::endl;
}

void InGameState::update(float delta_time)
{
    if  (!this->_initialized)
        return;

    this->_uiSystems.update(this->_uiRegistry, delta_time);
}

void on_play_clicked()
{
    std::cout << "[InGame] Play button clicked" << std::endl;
}

void InGameState::setup_ui()
{
    std::cout << "[InGame] Setting up UI" << std::endl;
}
