/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Game State Manager Implementation
*/

#include "GameStateManager.hpp"
#include <algorithm>
#include <memory>

void GameStateManager::push_state(const std::string& state_name, bool pause_current) {
    if (processing_states_) {
        pending_operations_.push_back({PendingOperation::Push, state_name, pause_current});
        return;
    }

    auto new_state = create_state(state_name);
    if (!new_state) {
        std::cerr << "Failed to create state: " << state_name << std::endl;
        return;
    }

    // Pause current state if requested
    if (!state_stack_.empty() && pause_current) {
        state_stack_.top()->pause();
    }

    // Push and enter new state
    state_stack_.push(new_state);
    new_state->enter();
}

void GameStateManager::pop_state() {
    if (processing_states_) {
        pending_operations_.push_back({PendingOperation::Pop, "", false});
        return;
    }

    if (state_stack_.empty()) {
        std::cerr << "Cannot pop state: stack is empty" << std::endl;
        return;
    }

    auto current_state = state_stack_.top();
    current_state->exit();
    state_stack_.pop();

    // Resume previous state if it exists
    if (!state_stack_.empty()) {
        state_stack_.top()->resume();
    }
}

void GameStateManager::change_state(const std::string& state_name) {
    if (processing_states_) {
        pending_operations_.push_back({PendingOperation::Change, state_name, false});
        return;
    }

    // Clear current stack
    while (!state_stack_.empty()) {
        state_stack_.top()->exit();
        state_stack_.pop();
    }

    // Push new state
    push_state(state_name, false);
}

void GameStateManager::clear_states() {
    if (processing_states_) {
        pending_operations_.push_back({PendingOperation::Clear, "", false});
        return;
    }

    while (!state_stack_.empty()) {
        state_stack_.top()->exit();
        state_stack_.pop();
    }
}

void GameStateManager::update(float delta_time) {
    process_pending_operations();

    if (state_stack_.empty()) return;

    processing_states_ = true;

    // Update states from bottom to top, but respect blocking
    std::vector<std::shared_ptr<IGameState>> states_to_update;
    auto temp_stack = state_stack_;

    while (!temp_stack.empty()) {
        auto state = temp_stack.top();
        states_to_update.push_back(state);
        temp_stack.pop();

        // If this state blocks updates, don't update states below it
        if (state->blocks_update()) {
            break;
        }
    }

    // Update in reverse order (bottom to top)
    std::reverse(states_to_update.begin(), states_to_update.end());
    for (auto& state : states_to_update) {
        state->update(delta_time);
    }

    processing_states_ = false;
}

void GameStateManager::render() {
    if (state_stack_.empty()) return;

    // Collect states to render (respecting blocking)
    std::vector<std::shared_ptr<IGameState>> states_to_render;
    auto temp_stack = state_stack_;

    while (!temp_stack.empty()) {
        auto state = temp_stack.top();
        states_to_render.push_back(state);
        temp_stack.pop();

        // If this state blocks rendering, don't render states below it
        if (state->blocks_render()) {
            break;
        }
    }

    // Render in reverse order (bottom to top)
    std::reverse(states_to_render.begin(), states_to_render.end());
    for (auto& state : states_to_render) {
        state->render();
    }
}

void GameStateManager::handle_input() {
    if (state_stack_.empty()) return;

    processing_states_ = true;
    // Only the top state handles input
    state_stack_.top()->handle_input();
    processing_states_ = false;
}

std::shared_ptr<IGameState> GameStateManager::get_current_state() const {
    return state_stack_.empty() ? nullptr : state_stack_.top();
}

std::string GameStateManager::get_current_state_name() const {
    auto current = get_current_state();
    return current ? current->get_name() : "None";
}

bool GameStateManager::has_state(const std::string& state_name) const {
    return state_factories_.find(state_name) != state_factories_.end();
}



void GameStateManager::process_pending_operations() {
    for (const auto& operation : pending_operations_) {
        switch (operation.operation) {
            case PendingOperation::Push:
                push_state(operation.state_name, operation.pause_current);
                break;
            case PendingOperation::Pop:
                pop_state();
                break;
            case PendingOperation::Change:
                change_state(operation.state_name);
                break;
            case PendingOperation::Clear:
                clear_states();
                break;
        }
    }
    pending_operations_.clear();
}

std::shared_ptr<IGameState> GameStateManager::create_state(const std::string& state_name) {
    auto it = state_factories_.find(state_name);
    if (it == state_factories_.end()) {
        std::cerr << "State factory not found: " << state_name << std::endl;
        return nullptr;
    }

    auto state = it->second();
    if (state) {
        auto self = std::make_shared<GameStateManager>(*this);
        state->set_state_manager(self);
    }
    return state;
}