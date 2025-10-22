#include "InGame.hpp"
#include "ECS/Systems/Position.hpp"
#include "Entity/Systems/Draw/Draw.hpp"
#include "ECS/Components/Animation.hpp"

#include "ECS/Components/Position.hpp"

// INFOS:
// It's a very basic implementation, must be upgraded.

void InGameState::enter()
{
    std::cout << "[InGame] Entering state" << std::endl;

    this->_systemLoader.load_components_from_so("build/lib/libECS.so", this->_registry);
    this->_systemLoader.load_system_from_so("build/lib/systems/libanimation_system.so", DLLoader::RenderSystem);
    this->_systemLoader.load_system_from_so("build/lib/systems/libposition_system.so", DLLoader::LogicSystem);

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

    this->_systemLoader.update_all_systems(this->_registry, delta_time, DLLoader::LogicSystem);
}

void InGameState::setup_ui()
{
    std::cout << "[InGame] Setting up UI" << std::endl;
    this->_playerEntity = this->_registry.spawn_entity();

    auto componentFactory = this->_systemLoader.get_factory();
    if (componentFactory) {
        componentFactory->create_component<position>(this->_registry, this->_playerEntity, 100.0f, 200.0f);
        componentFactory->create_component<animation>(this->_registry, this->_playerEntity, "Games/RType/Assets/dedsec_eyeball-Sheet.png", 400, 400, 0.25, 0.25, 0, true);
    }
}
