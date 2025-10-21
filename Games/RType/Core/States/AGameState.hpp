#pragma once

#include "IGameState.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Systems/UISystem.hpp"
#include "ECS/Components/UIComponent.hpp"
#include "ECS/UI/Components/Button.hpp"
#include "ECS/UI/Components/Panel.hpp"
#include "ECS/UI/Components/Text.hpp"
#include <memory>

#include "Core/States/GameStateManager.hpp"
class AGameState : public IGameState {
    public:
        AGameState();
        virtual ~AGameState() = default;

        void render() override;

        virtual void setup_ui() = 0;
        void cleanup_ui() override;

        void handle_input() override;

        virtual bool blocks_update() const override { return true; }
        virtual bool blocks_render() const override { return true; }
    protected:
        std::shared_ptr<GameStateManager> _stateManager;
        std::shared_ptr<registry> _uiRegistry;
        std::shared_ptr<UI::UISystem> _uiSystems;

        bool _initialized{false};

        void set_state_manager(std::shared_ptr<GameStateManager> manager) override { _stateManager = manager; }
        void set_ui_registry(std::shared_ptr<registry> uiRegistry) override { _uiRegistry = uiRegistry; }
};

inline std::string state_type_to_string(IGameState::GameStateType type) {
    switch (type) {
        case IGameState::GameStateType::MenusBackground: return "MenusBackground";
        case IGameState::GameStateType::MainMenu: return "MainMenu";
        // case IGameState::GameStateType::Settings: return "Settings";
        // case IGameState::GameStateType::Lobby: return "Lobby";
        case IGameState::GameStateType::InGame: return "InGame";
        // case IGameState::GameStateType::GameOver: return "GameOver";
        // case IGameState::GameStateType::Loading: return "Loading";
        // case IGameState::GameStateType::WaitingLobby: return "WaitingLobby";
        default: return "Unknown";
    }
}