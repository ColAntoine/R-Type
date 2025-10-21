#include "InGame.hpp"

void InGameState::enter()
{
    std::cout << "[InGame] Entering state" << std::endl;

    _uiRegistry.register_component<UI::UIComponent>();
    _uiRegistry.register_component<RType::UIMainPanel>();
    _uiRegistry.register_component<RType::UITitleText>();
    _uiRegistry.register_component<RType::UIPlayButton>();
    _uiRegistry.register_component<RType::UISettingsButton>();
    _uiRegistry.register_component<RType::UIQuitButton>();
}