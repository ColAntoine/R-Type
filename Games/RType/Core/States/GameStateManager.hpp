/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Game State Manager - Manages game states and transitions
*/

#pragma once

#include "Core/States/GameState.hpp"
#include <stack>
#include <memory>
#include <unordered_map>
#include <functional>
#include <iostream>

class GameStateManager {
    private:
        std::stack<std::shared_ptr<IGameState>> state_stack_;
        std::unordered_map<std::string, std::function<std::shared_ptr<IGameState>()>> state_factories_;

        // Pending state operations (processed at the start of next frame to avoid iterator invalidation)
        enum class PendingOperation {
            Push,
            Pop,
            Change,
            Clear
        };

        struct StateOperation {
            PendingOperation operation;
            std::string state_name;
            bool pause_current;
        };

        std::vector<StateOperation> pending_operations_;
        bool processing_states_{false};

    public:
        GameStateManager() = default;
        ~GameStateManager() = default;

        // State factory registration
        template<typename T>
        void register_state(const std::string& name) {
            state_factories_[name] = []() -> std::shared_ptr<IGameState> {
                return std::make_shared<T>();
            };
        }

        // Register state with custom factory function
        void register_state_with_factory(const std::string& name, std::function<std::shared_ptr<IGameState>()> factory) {
            state_factories_[name] = factory;
        }

        // State stack operations
        void push_state(const std::string& state_name, bool pause_current = true);
        void pop_state();
        void change_state(const std::string& state_name);
        void clear_states();

        // Update and render
        void update(float delta_time);
        void render();
        void handle_input();

        // State information
        bool is_empty() const { return state_stack_.empty(); }
        size_t get_state_count() const { return state_stack_.size(); }
        std::shared_ptr<IGameState> get_current_state() const;
        std::string get_current_state_name() const;

        // Utility methods
        bool has_state(const std::string& state_name) const;
        void print_state_stack() const;

    private:
        void process_pending_operations();
        std::shared_ptr<IGameState> create_state(const std::string& state_name);
};