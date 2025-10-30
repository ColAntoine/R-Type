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
    // If states are currently being processed (update/render/input), enqueue operation
    if (processing_states_.load()) {
        std::lock_guard<std::mutex> lock(mutex_);
        pending_operations_.push_back({PendingOperation::Push, state_name, pause_current});
        return;
    }

    auto new_state = create_state(state_name);
    if (!new_state) {
        std::cerr << "Failed to create state: " << state_name << std::endl;
        return;
    }

    // Modify stack under lock, but call lifecycle methods outside the lock to avoid deadlocks
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!state_stack_.empty() && pause_current) {
            state_stack_.top()->pause();
        }
        state_stack_.push(new_state);
    }

    new_state->enter();
}

void GameStateManager::pop_state() {
    if (processing_states_.load()) {
        std::lock_guard<std::mutex> lock(mutex_);
        pending_operations_.push_back({PendingOperation::Pop, "", false});
        return;
    }

    std::shared_ptr<IGameState> to_exit;
    std::shared_ptr<IGameState> to_resume;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (state_stack_.empty()) {
            std::cerr << "Cannot pop state: stack is empty" << std::endl;
            return;
        }
        to_exit = state_stack_.top();
        state_stack_.pop();
        if (!state_stack_.empty()) {
            to_resume = state_stack_.top();
        }
    }

    // Call lifecycle methods outside lock
    if (to_exit) to_exit->exit();
    if (to_resume) to_resume->resume();
}

void GameStateManager::change_state(const std::string& state_name) {
    if (processing_states_.load()) {
        std::lock_guard<std::mutex> lock(mutex_);
        pending_operations_.push_back({PendingOperation::Change, state_name, false});
        return;
    }

    // Pop all states under lock and collect them to call exit() outside the lock
    std::vector<std::shared_ptr<IGameState>> to_exit;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!state_stack_.empty()) {
            to_exit.push_back(state_stack_.top());
            state_stack_.pop();
        }
    }
    for (auto &s : to_exit) {
        if (s) s->exit();
    }

    // Push new state (will handle locking)
    push_state(state_name, false);
}

void GameStateManager::clear_states() {
    if (processing_states_.load()) {
        std::lock_guard<std::mutex> lock(mutex_);
        pending_operations_.push_back({PendingOperation::Clear, "", false});
        return;
    }

    std::vector<std::shared_ptr<IGameState>> to_exit;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!state_stack_.empty()) {
            to_exit.push_back(state_stack_.top());
            state_stack_.pop();
        }
    }
    for (auto &s : to_exit) {
        if (s) s->exit();
    }
}

void GameStateManager::update(float delta_time) {
    process_pending_operations();

    // Snapshot stack under lock to avoid races with other threads
    std::stack<std::shared_ptr<IGameState>> temp_stack;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (state_stack_.empty()) return;
        temp_stack = state_stack_;
    }

    processing_states_.store(true);

    // Update states from bottom to top, but respect blocking
    std::vector<std::shared_ptr<IGameState>> states_to_update;
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

    processing_states_.store(false);
}

void GameStateManager::render() {
    // Snapshot stack under lock
    std::stack<std::shared_ptr<IGameState>> temp_stack;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (state_stack_.empty()) return;
        temp_stack = state_stack_;
    }

    // Collect states to render (respecting blocking)
    std::vector<std::shared_ptr<IGameState>> states_to_render;
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
    std::shared_ptr<IGameState> top;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (state_stack_.empty()) return;
        top = state_stack_.top();
    }

    processing_states_.store(true);
    if (top) top->handle_input();
    processing_states_.store(false);
}

std::shared_ptr<IGameState> GameStateManager::get_current_state() const {
    std::lock_guard<std::mutex> lock(mutex_);
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
    // Snapshot pending operations under lock to avoid races with other threads
    std::vector<StateOperation> ops;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ops.swap(pending_operations_);
    }

    for (const auto& operation : ops) {
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
}

std::shared_ptr<IGameState> GameStateManager::create_state(const std::string& state_name) {
    auto it = state_factories_.find(state_name);
    if (it == state_factories_.end()) {
        std::cerr << "State factory not found: " << state_name << std::endl;
        return nullptr;
    }

    auto state = it->second();
    if (state) {
        state->set_state_manager(this);
    }
    return state;
}