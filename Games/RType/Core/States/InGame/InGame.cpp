#include "InGame.hpp"
#include "ECS/Systems/Position.hpp"
#include "Entity/Systems/Draw/Draw.hpp"
#include "ECS/Components/Animation.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/AssetManager/AssetManager.hpp"
#include "Core/Client/Network/NetworkService.hpp"
#include "Core/Client/Network/ClientService.hpp"
#include "Core/Server/Protocol/Protocol.hpp"

#include <string>
#include <raylib.h>

// INFOS:
// It's a very basic implementation, must be upgraded.

InGameState::InGameState() {
}

void InGameState::enter()
{
    std::cout << "[InGame] Entering state" << std::endl;

    auto* network_manager = RType::Network::get_network_manager();

    if (network_manager) {
        // Multiplayer mode - use shared registry/loader from NetworkManager
        std::cout << "[InGame] Using SHARED registry (multiplayer mode)" << std::endl;
        active_registry_ = &network_manager->get_registry();
        active_loader_ = &network_manager->get_loader();
    } else {
        // Solo mode - use local registry/loader
        std::cout << "[InGame] Using LOCAL registry (solo mode)" << std::endl;
        active_registry_ = &_registry;
        active_loader_ = &_systemLoader;

        active_loader_->load_components_from_so("build/lib/libECS.so", *active_registry_);
        active_loader_->load_system_from_so("build/lib/systems/libgame_EnemySpawnSystem.so", DLLoader::LogicSystem);
        createPlayer();
    }

    active_loader_->load_system_from_so("build/lib/systems/libanimation_system.so", DLLoader::RenderSystem);
    active_loader_->load_system_from_so("build/lib/systems/libgame_Draw.so", DLLoader::RenderSystem);

    active_loader_->load_system_from_so("build/lib/systems/libposition_system.so", DLLoader::LogicSystem);
    active_loader_->load_system_from_so("build/lib/systems/libcollision_system.so", DLLoader::LogicSystem);
    active_loader_->load_system_from_so("build/lib/systems/libgame_Control.so", DLLoader::LogicSystem);
    active_loader_->load_system_from_so("build/lib/systems/libgame_Shoot.so", DLLoader::LogicSystem);
    active_loader_->load_system_from_so("build/lib/systems/libgame_GravitySys.so", DLLoader::LogicSystem);
    active_loader_->load_system_from_so("build/lib/systems/libgame_EnemyCleanup.so", DLLoader::LogicSystem);
    active_loader_->load_system_from_so("build/lib/systems/libgame_EnemyAI.so", DLLoader::LogicSystem);
    active_loader_->load_system_from_so("build/lib/systems/libgame_LifeTime.so", DLLoader::LogicSystem);
    active_loader_->load_system_from_so("build/lib/systems/libgame_Health.so", DLLoader::LogicSystem);
    active_loader_->load_system_from_so("build/lib/systems/libgame_ParabolSys.so", DLLoader::LogicSystem);

    std::cout << "[InGame] Systems loaded" << std::endl;

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

    handle_input();

    // Use the resolved references from enter()
    active_loader_->update_all_systems(*active_registry_, delta_time, DLLoader::LogicSystem);
    active_loader_->update_all_systems(*active_registry_, delta_time, DLLoader::RenderSystem);

}void InGameState::setup_ui()
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

void InGameState::handle_input()
{
    auto* network_manager = RType::Network::get_network_manager();
    if (network_manager) {
        // Check for arrow key input
        uint8_t input_state = 0;
        if (IsKeyDown(KEY_UP))    input_state |= static_cast<uint8_t>(RType::Protocol::InputFlags::UP);
        if (IsKeyDown(KEY_DOWN))  input_state |= static_cast<uint8_t>(RType::Protocol::InputFlags::DOWN);
        if (IsKeyDown(KEY_LEFT))  input_state |= static_cast<uint8_t>(RType::Protocol::InputFlags::LEFT);
        if (IsKeyDown(KEY_RIGHT)) input_state |= static_cast<uint8_t>(RType::Protocol::InputFlags::RIGHT);

        // Only send if input state changed
        static uint8_t last_input_state = 0;
        if (input_state != last_input_state) {
            auto client = RType::Network::get_client();
            if (client) {
                uint32_t player_token = client->get_session_token();

                RType::Protocol::PlayerInput input_msg;
                input_msg.player_token = player_token;
                input_msg.input_state = input_state;
                input_msg.timestamp = GetTime() * 1000; // Convert to milliseconds

                auto packet = RType::Protocol::create_packet(
                    static_cast<uint8_t>(RType::Protocol::GameMessage::PLAYER_INPUT),
                    input_msg
                );

                client->send_packet(reinterpret_cast<const char*>(packet.data()), packet.size());
            }
            last_input_state = input_state;
        }
    }
}