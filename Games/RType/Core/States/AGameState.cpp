/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Abstract Game State Implementation
*/

#include "AGameState.hpp"
#include "ECS/Zipper.hpp"
#include <iostream>

AGameState::AGameState() {}

void AGameState::render()
{
    if (!this->_initialized)
        return;
    this->_uiSystems.render(this->_uiRegistry);
}

void AGameState::cleanup_ui()
{
    // Collect all entity IDs first to avoid modifying while iterating
    std::vector<entity> entities_to_cleanup;

    auto* ui_components = _uiRegistry.get_if<UI::UIComponent>();
    if (ui_components) {
        for (auto [comp, ent] : zipper(*ui_components)) {
            entities_to_cleanup.push_back(entity(ent));
        }
    }

    // Now remove all components from collected entities
    for (auto ent : entities_to_cleanup) {
        this->_uiRegistry.kill_entity(ent);
    }

    std::cout << "[AGameState] UI cleanup complete (" << entities_to_cleanup.size() << " entities removed)" << std::endl;
}

void AGameState::handle_input() {
    if (this->_initialized == false)
        return;
    this->_uiSystems.process_input(this->_uiRegistry);
}
