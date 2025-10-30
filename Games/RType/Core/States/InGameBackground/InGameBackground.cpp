#include "InGameBackground.hpp"
#include <random>
#include "ECS/Renderer/RenderManager.hpp"

void InGameBackground::enter()
{
    #ifdef _WIN32
        const std::string ext = ".dll";
    #else
        const std::string ext = ".so";
    #endif

    _systemLoader->load_components("build/lib/libECS" + ext, _registry);
    _systemLoader->load_system("build/lib/systems/librender_UISystem" + ext, ILoader::RenderSystem);
    _systemLoader->load_system("build/lib/systems/libgame_StarBg" + ext, ILoader::LogicSystem);

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

    _systemLoader->update_all_systems(_registry, delta_time, ILoader::LogicSystem);
}
