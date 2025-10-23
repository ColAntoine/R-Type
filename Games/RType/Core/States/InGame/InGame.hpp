/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** In-Game State - Wraps existing game logic
*/

#pragma once

#include "Core/States/GameState.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Systems/UISystem.hpp"
#include "ECS/Components/UIComponent.hpp"
#include "ECS/UI/Components/Text.hpp"
#include "ECS/DLLoader.hpp"
#include <memory>
#include <vector>

// Tag components for InGame HUD elements
namespace RType {
    struct UIFPSText : public IComponent {};
    struct UIPlayerInfo : public IComponent {};
    struct UIConnectionStatus : public IComponent {};
    struct UIPositionText : public IComponent {};
    struct UIScoreText : public IComponent {};
}

class InGameState : public IGameState {
    private:
        bool initialized_{false};
        bool paused_{false};

    // ECS UI system for HUD
        registry ui_registry_;
        registry &ecs_registry_;
        DLLoader* ecs_loader_;
        IComponentFactory* factory_;
        UI::UISystem ui_system_;

        // In-game background state
        float bg_time_{0.0f};
        struct DataStream { float x; float y; float speed; float length; int chars; };
        std::vector<DataStream> bg_streams_;
        int bg_stream_count_{0};

    public:
        InGameState(registry &ecs, DLLoader* loader);
        ~InGameState() override = default;

        // IGameState implementation
        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void update(float delta_time) override;
        void render() override;
        void handle_input() override;

        std::string get_name() const override { return "InGame"; }
        bool blocks_update() const override { return true; }
        bool blocks_render() const override { return true; }

    private:
        void setup_hud();
        void cleanup_hud();
        void update_hud();
        // Render helpers
        void render_falling_background();
};