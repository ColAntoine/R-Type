/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Abstract Game State Implementation
*/

#include "AGameState.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Messaging/MessagingManager.hpp"
#include "ECS/Messaging/Events/Event.hpp"
#include <iostream>

#if defined(_MSC_VER)
  #define ATTR_MAYBE_UNUSED [[maybe_unused]]
#else
  #define ATTR_MAYBE_UNUSED __attribute__((unused))
#endif

#ifdef _WIN32
    #include "ECS/WinLoader.hpp"
    using PlatformLoader = WinLoader;
#else
    #include "ECS/LinuxLoader.hpp"
    using PlatformLoader = LinuxLoader;
#endif

AGameState::AGameState() 
{
    _systemLoader = std::make_unique<PlatformLoader>();
}

AGameState::AGameState(registry* shared_registry, ILoader* shared_loader)
    : _shared_registry(shared_registry), _shared_loader(shared_loader) 
{
    if (!_shared_loader)
        _systemLoader = std::make_unique<PlatformLoader>();
}

AGameState::~AGameState()
{
    cleanup_ui();
}

void AGameState::render()
{
    if (!this->_initialized)
        return;
    
    // Use shared registry/loader if available
    registry& reg = _shared_registry ? *_shared_registry : _registry;
    ILoader& loader = _shared_loader ? *_shared_loader : *_systemLoader;
    
    loader.update_all_systems(reg, 0.0f, ILoader::RenderSystem);
}

void AGameState::update(float delta_time)
{
    if (!this->_initialized)
        return;
    
    // Use shared registry/loader if available
    registry& reg = _shared_registry ? *_shared_registry : _registry;
    ILoader& loader = _shared_loader ? *_shared_loader : *_systemLoader;
    
    loader.update_all_systems(reg, delta_time, ILoader::LogicSystem);
}

void AGameState::cleanup_ui()
{
    // Use shared registry if available
    registry& reg = _shared_registry ? *_shared_registry : _registry;
    
    // Collect all entity IDs first to avoid modifying while iterating
    std::vector<entity> entities_to_cleanup;

    auto* ui_components = reg.get_if<UI::UIComponent>();
    if (ui_components) {
        for (auto [comp, ent] : zipper(*ui_components)) {
            entities_to_cleanup.push_back(entity(ent));
        }
    }

    // Now remove all components from collected entities
    for (entity ent : entities_to_cleanup) {
        reg.kill_entity(ent);
    }
}

void AGameState::handle_input() {
    if (this->_initialized == false)
        return;
    // this->_uiSystems.process_input(this->_registry);
}

void AGameState::subscribe_to_ui_event()
{
    auto& eventBus = MessagingManager::instance().get_event_bus();
    _uiEventCallbackId = eventBus.subscribe(
        EventTypes::SCREEN_PARAMETERS_CHANGED,
        [this](ATTR_MAYBE_UNUSED const Event& event) {
            this->cleanup_ui();
            this->setup_ui();
        }
    );
}

