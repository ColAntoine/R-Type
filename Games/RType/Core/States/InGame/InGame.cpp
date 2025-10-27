#include "InGame.hpp"
#include "ECS/Systems/Position.hpp"
#include "Entity/Systems/Draw/Draw.hpp"
#include "ECS/Components/Animation.hpp"

#include "ECS/Components/Position.hpp"

#include <string>

// INFOS:
// It's a very basic implementation, must be upgraded.

void InGameState::enter()
{
    std::cout << "[InGame] Entering state" << std::endl;

    // Use shared registry if available (for multiplayer), otherwise use local registry
    registry& reg = _shared_registry ? *_shared_registry : _registry;
    DLLoader& loader = _shared_loader ? *_shared_loader : _systemLoader;

    if (_shared_registry) {
        std::cout << "[InGame] Using SHARED registry (multiplayer mode)" << std::endl;
    } else {
        std::cout << "[InGame] Using LOCAL registry (solo mode)" << std::endl;
        loader.load_system_from_so("build/lib/systems/libgame_EnemySpawnSystem.so", DLLoader::LogicSystem);
        createPlayer();
    }

    loader.load_components_from_so("build/lib/libECS.so", reg);
    loader.load_system_from_so("build/lib/systems/libanimation_system.so", DLLoader::RenderSystem);
    loader.load_system_from_so("build/lib/systems/libgame_Draw.so", DLLoader::RenderSystem);

    loader.load_system_from_so("build/lib/systems/libposition_system.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libcollision_system.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_Control.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_Shoot.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_GravitySys.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_EnemyCleanup.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_EnemyAI.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_LifeTime.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_Health.so", DLLoader::LogicSystem);

    // Debug: Check how many entities exist in the registry
    std::cout << "[InGame] Registry has entities at startup" << std::endl;

    // TODO: Create a condition that only work in solo
    loader.load_system_from_so("build/lib/systems/libgame_EnemySpawnSystem.so", DLLoader::LogicSystem);
    createPlayer();
    setup_ui();
    _initialized = true;
}

void InGameState::exit()
{
    _initialized = false;
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
    if  (!_initialized)
        return;

    // Use shared loader if available
    DLLoader& loader = _shared_loader ? *_shared_loader : _systemLoader;
    registry& reg = _shared_registry ? *_shared_registry : _registry;

    loader.update_all_systems(reg, delta_time, DLLoader::LogicSystem);
    loader.update_all_systems(_registry, delta_time, DLLoader::RenderSystem);
}

void InGameState::setup_ui()
{
    std::cout << "[InGame] Setting up UI" << std::endl;
}

void InGameState::createPlayer()
{
    auto componentFactory = _systemLoader.get_factory();

    _playerEntity = _registry.spawn_entity();
    if (componentFactory) {
        componentFactory->create_component<position>(_registry, _playerEntity, PLAYER_SPAWN_X, PLAYER_SPAWN_Y);
        componentFactory->create_component<animation>(_registry, _playerEntity,  std::string(RTYPE_PATH_ASSETS) + "dedsec_eyeball-Sheet.png", 400, 400, 0.25, 0.25, 0, true);
        componentFactory->create_component<controllable>(_registry, _playerEntity, 300.f);      // ! SPEED TO BE REDUCED
        componentFactory->create_component<Weapon>(_registry, _playerEntity);
        componentFactory->create_component<collider>(_registry, _playerEntity);
        componentFactory->create_component<Score>(_registry, _playerEntity);
        componentFactory->create_component<Health>(_registry, _playerEntity);
    }
}
