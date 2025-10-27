#include "InGameBackground.hpp"
#include <random>
#include "ECS/Renderer/RenderManager.hpp"

void InGameBackground::enter()
{
    _systemLoader.load_components_from_so("build/lib/libECS.so", _registry);
    _systemLoader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);
    _systemLoader.load_system_from_so("build/lib/systems/libgame_StarBg.so", DLLoader::LogicSystem);

    setup_ui();
    _initialized = true;
}

void InGameBackground::exit()
{
    _initialized = false;
}

void InGameBackground::pause()
{

}

void InGameBackground::resume()
{

}


void InGameBackground::setup_ui()
{

}

void InGameBackground::update(float delta_time)
{
    if  (!_initialized)
        return;

    _systemLoader.update_all_systems(_registry, delta_time, DLLoader::LogicSystem);
}
