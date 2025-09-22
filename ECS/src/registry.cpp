/*
** EPITECH PROJECT, 2025
** asd
** File description:
** asd
*/

#include "ecs/registry.hpp"

#include <algorithm>

registry::registry() = default;
registry::~registry() = default;

entity registry::spawn_entity() {
    if (!_free_ids.empty()) {
        auto id = _free_ids.back();
        _free_ids.pop_back();
        return entity(id);
    }
    return entity(_next_id++);
}

entity registry::entity_from_index(std::size_t idx) const {
    return entity(idx);
}

void registry::kill_entity(entity const& e) {
    for (auto &fn : _erasers) fn(e);
    _free_ids.push_back(static_cast<size_t>(e));
}

void registry::run_systems() {
    for (auto &s : _systems) s(*this);
}
