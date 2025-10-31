#include "InGame.hpp"
#include "Entity/Components/Player/Player.hpp"
#include "ECS/Components/Animation.hpp"
#include "ECS/Components/Position.hpp"
#include "Entity/Components/Input/Input.hpp"
#include "Core/Client/Network/ClientService.hpp"
#include "Core/Client/Network/NetworkService.hpp"
#include "Core/Server/Protocol/Protocol.hpp"
#include "Core/Config/Config.hpp"
#include "ECS/Messaging/MessagingManager.hpp"
#include "Core/KeyBindingManager/KeyBindingManager.hpp"

#include <string>
#include <random>

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

        // Generate random seed for solo play (deterministic for potential replay features)
        std::random_device rd;
        unsigned int solo_seed = rd();
        _registry.set_random_seed(solo_seed);
        std::cout << "[InGame] Generated solo game seed: " << solo_seed << std::endl;
    }

    // Load components first (needed for both solo and multiplayer)
    loader.load_components_from_so("build/lib/libECS.so", reg);

    // Load render systems
    loader.load_components_from_so("build/lib/libECS.so", _registry);
    loader.load_system_from_so("build/lib/systems/libanimation_system.so", DLLoader::RenderSystem);
    loader.load_system_from_so("build/lib/systems/libgame_Draw.so", DLLoader::RenderSystem);
    loader.load_system_from_so("build/lib/systems/libsprite_system.so", DLLoader::RenderSystem);
    loader.load_system_from_so("build/lib/systems/libgame_PUpAnimationSys.so", DLLoader::RenderSystem);
    loader.load_system_from_so("build/lib/systems/librender_UISystem.so", DLLoader::RenderSystem);

    loader.load_system_from_so("build/lib/systems/libposition_system.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libcollision_system.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_KeyInput.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_Control.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_Shoot.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_GravitySys.so", DLLoader::LogicSystem);

    // ! unload the enemy for boss testing
    loader.load_system_from_so("build/lib/systems/libgame_EnemyCleanup.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_EnemyAI.so", DLLoader::LogicSystem);

    loader.load_system_from_so("build/lib/systems/libgame_EnemySpawnSystem.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_BossSys.so", DLLoader::LogicSystem);

    loader.load_system_from_so("build/lib/systems/libgame_LifeTime.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_Health.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_ParabolSys.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_FollowingSys.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_WaveSys.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_PowerUpSys.so", DLLoader::LogicSystem);
    loader.load_system_from_so("build/lib/systems/libgame_GameLogic.so", DLLoader::LogicSystem);
    // Debug: Check how many entities exist in the registry
    std::cout << "[InGame] Registry has entities at startup" << std::endl;

    // Create player
    // In solo mode: create immediately
    // In multiplayer mode: DON'T create here - let the network create it via on_player_spawn()
    if (!_shared_registry) {
        std::cout << "[InGame] Solo mode - Creating player..." << std::endl;
        createPlayer();
    } else {
        std::cout << "[InGame] Multiplayer mode - Player will be created by network (PLAYER_SPAWN message)" << std::endl;
    }

    auto &eventBus = MessagingManager::instance().get_event_bus();
    Event event(EventTypes::SET_KEY_BINDINGS);

    const auto &keyBinds = KeyBindingManager::instance().getKeyBindings();

    event.set("keyBindings", keyBinds);
    eventBus.emit(event);

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

    // Use shared loader and registry if available (for multiplayer)
    DLLoader& loader = _shared_loader ? *_shared_loader : _systemLoader;
    registry& reg = _shared_registry ? *_shared_registry : _registry;

    // Send input to server if connected
    handle_input();

    // Update both logic and render systems with the correct registry
    loader.update_all_systems(reg, delta_time, DLLoader::LogicSystem);
    loader.update_all_systems(reg, delta_time, DLLoader::RenderSystem);
}

void InGameState::handle_input()
{
    auto* network_manager = RType::Network::get_network_manager();
    if (network_manager) {
        const auto &keyBinds = KeyBindingManager::instance().getKeyBindings();
        // Check for arrow key input
        uint8_t input_state = 0;
        if (keyBinds.count("move_up") && IsKeyDown(keyBinds.at("move_up")))
            input_state |= static_cast<uint8_t>(RType::Protocol::InputFlags::UP);
        if (keyBinds.count("move_down") && IsKeyDown(keyBinds.at("move_down")))
            input_state |= static_cast<uint8_t>(RType::Protocol::InputFlags::DOWN);
        if (keyBinds.count("move_left") && IsKeyDown(keyBinds.at("move_left")))
            input_state |= static_cast<uint8_t>(RType::Protocol::InputFlags::LEFT);
        if (keyBinds.count("move_right") && IsKeyDown(keyBinds.at("move_right")))
            input_state |= static_cast<uint8_t>(RType::Protocol::InputFlags::RIGHT);

        // Only send if input state changed
        static uint8_t last_input_state = 0;
        if (input_state != last_input_state) {
            auto client = RType::Network::get_client();
            if (client) {
                uint32_t player_token = client->get_session_token();
                // Only send input to server when we have an assigned session token (i.e. actually connected)
                if (player_token == 0) {
                    // not connected - do not send input in solo mode
                } else {
                    RType::Protocol::PlayerInput input_msg;
                    input_msg.player_token = player_token;
                    input_msg.input_state = input_state;
                    input_msg.timestamp = static_cast<uint32_t>(GetTime() * 1000); // Convert to milliseconds

                    auto packet = RType::Protocol::create_packet(
                        static_cast<uint8_t>(RType::Protocol::GameMessage::PLAYER_INPUT),
                        input_msg
                    );

                    client->send_packet(reinterpret_cast<const char*>(packet.data()), packet.size());
                }
            }
            last_input_state = input_state;
        }
    }
}

void InGameState::setup_ui()
{
    std::cout << "[InGame] Setting up UI" << std::endl;
}

void InGameState::createPlayer()
{
    std::cout << "[InGame] createPlayer() called" << std::endl;

    // Use shared registry/loader if available (for multiplayer), otherwise use local
    registry& reg = _shared_registry ? *_shared_registry : _registry;
    DLLoader& loader = _shared_loader ? *_shared_loader : _systemLoader;
    auto componentFactory = loader.get_factory();

    if (!componentFactory) {
        std::cerr << "[InGame] ERROR: Component factory is NULL!" << std::endl;
        return;
    }

    _playerEntity = reg.spawn_entity();
    std::cout << "[InGame] Spawned player entity: " << static_cast<size_t>(_playerEntity) << std::endl;

    if (componentFactory) {
        componentFactory->create_component<position>(reg, _playerEntity, PLAYER_SPAWN_X, PLAYER_SPAWN_Y);
        componentFactory->create_component<animation>(reg, _playerEntity,  std::string(RTYPE_PATH_ASSETS) + "dedsec_eyeball-Sheet.png", 400, 400, 0.25, 0.25, 0, true);
        componentFactory->create_component<controllable>(reg, _playerEntity, 300.f);
        componentFactory->create_component<Input>(reg, _playerEntity);
        componentFactory->create_component<Weapon>(reg, _playerEntity);
        componentFactory->create_component<collider>(reg, _playerEntity, 100.f, 100.f, -50.f, -50.f);
        componentFactory->create_component<Score>(reg, _playerEntity);
        componentFactory->create_component<Health>(reg, _playerEntity);
        componentFactory->create_component<Player>(reg, _playerEntity);
    }
}

