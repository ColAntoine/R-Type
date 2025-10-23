#include "InGameHud.hpp"

void InGameHudState::enter()
{
    _systemLoader.load_components_from_so("build/lib/libECS.so", _registry);
    _systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);


    this->_registry.register_component<UI::UIButton>();

    setup_ui();
    _initialized = true;
}

void InGameHudState::exit()
{
    cleanup_ui();
    _initialized = false;
}

void InGameHudState::pause()
{

}

void InGameHudState::resume()
{

}


void InGameHudState::setup_ui()
{
    auto playButton = ButtonBuilder()
        .centered(200)
        .size(400, 100)
        .text("PLAY")
        .blue()
        .textColor(WHITE)
        .fontSize(24)
        .border(2, WHITE)
        .onClick([this]() {
            std::cout << "clicked" << std::endl;
        }
    )
    .build(SCREEN_WIDTH, SCREEN_HEIGHT);
    auto playButtonEntity = this->_registry.spawn_entity();
    this->_registry.add_component(playButtonEntity, UI::UIComponent(playButton));
}

void InGameHudState::update(float delta_time)
{
    if  (!_initialized)
        return;
    _systemLoader.update_all_systems(_registry, delta_time, DLLoader::LogicSystem);
}

