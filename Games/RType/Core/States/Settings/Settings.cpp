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
    this->cleanup_ui();
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
    if (!this->_initialized)
        return;
    this->_systemLoader.update_all_systems(this->_registry, delta_time, DLLoader::LogicSystem);
}

void SettingsState::setup_ui()
{
    auto &renderManager = RenderManager::instance();
    auto winInfos = renderManager.get_screen_infos();
}
