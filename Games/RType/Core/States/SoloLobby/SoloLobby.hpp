/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** SoloLobby - Offline game mode for testing engine features
*/

#pragma once

#include "Core/States/GameState.hpp"
#include "UI/UIManager.hpp"
#include "UI/Components/UIButton.hpp"
#include "UI/Components/UIText.hpp"
#include "UI/Components/UIPanel.hpp"
#include "ECS/Registry.hpp"
#include "ECS/DLLoader.hpp"
#include "ECS/ComponentFactory.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/Audio/AudioManager.hpp"
#include "ECS/Physics/PhysicsManager.hpp"
#include "ECS/Messaging/EventBus.hpp"
#include "ECS/Messaging/Events/Event.hpp"
#include <memory>
#include <random>

class SoloLobbyState : public IGameState {
    private:
        UIManager ui_manager_;
        bool initialized_{false};

        // ECS and Engine Systems
        registry ecs_registry_;
        DLLoader system_loader_;
        DLLoader component_loader_;
        IComponentFactory* component_factory_{nullptr};

        // Test entities
        entity player_entity_;
        entity enemy_entity_;
        entity background_entity_;

        // Demo state
        float elapsed_time_{0.0f};
        bool audio_test_played_{false};
        int test_phase_{0};  // 0=init, 1=rendering, 2=physics, 3=audio, 4=all

        // Messaging system
        EventBus event_bus_;
        std::vector<EventBus::CallbackId> event_subscriptions_;

        // UI state
        std::string status_message_;
        std::vector<std::string> log_messages_;

    public:
        SoloLobbyState() = default;
        ~SoloLobbyState() override = default;

        // IGameState implementation
        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void update(float delta_time) override;
        void render() override;
        void handle_input() override;

        std::string get_name() const override { return "SoloLobby"; }

    private:
        void setup_ui();
        void cleanup_ui();
        void setup_ecs_demo();
        void setup_event_bus();
        void load_systems();
        void create_test_entities();
        void add_log(const std::string& message);

        // Button callbacks
        void on_test_renderer_clicked();
        void on_test_physics_clicked();
        void on_test_audio_clicked();
        void on_test_all_clicked();
        void on_back_clicked();
};
