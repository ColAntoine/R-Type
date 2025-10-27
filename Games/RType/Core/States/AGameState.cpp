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

AGameState::AGameState() {}

AGameState::~AGameState()
{
    cleanup_ui();
}

void AGameState::render()
{
    if (!this->_initialized)
        return;
    this->_systemLoader.update_all_systems(this->_registry, 0.0f, DLLoader::RenderSystem);
}

void AGameState::update(float delta_time)
{
    if (!this->_initialized)
        return;
    this->_systemLoader.update_all_systems(this->_registry, delta_time, DLLoader::LogicSystem);
}

void AGameState::cleanup_ui()
{
    // Collect all entity IDs first to avoid modifying while iterating
    std::vector<entity> entities_to_cleanup;

    auto* ui_components = _registry.get_if<UI::UIComponent>();
    if (ui_components) {
        for (auto [comp, ent] : zipper(*ui_components)) {
            entities_to_cleanup.push_back(entity(ent));
        }
    }

    // Now remove all components from collected entities
    for (auto ent : entities_to_cleanup) {
        this->_registry.kill_entity(ent);
    }

    std::cout << "[AGameState] UI cleanup complete (" << entities_to_cleanup.size() << " entities removed)" << std::endl;
}

void AGameState::handle_input() {
    if (this->_initialized == false)
        return;
    // this->_uiSystems.process_input(this->_registry);
}

void AGameState::subscribe_to_ui_event()
{
    auto& eventBus = MessagingManager::instance().get_event_bus();
    _uiEventCallbackId = eventBus.subscribe(EventTypes::SCREEN_PARAMETERS_CHANGED, [this](__attribute_maybe_unused__ const Event& event) {
        std::cout << "Screen Parameter changed!!!" << std::endl;
        this->cleanup_ui();
        this->setup_ui();
    });
}

