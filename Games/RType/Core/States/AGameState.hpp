#pragma once

#include "IGameState.hpp"
#include "ECS/Registry.hpp"
#include "ECS/DLLoader.hpp"
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
        virtual ~AGameState() override;

        void render() override;
        virtual void update(float delta_time) override;

        virtual void setup_ui() = 0;
        void cleanup_ui() override;

        void handle_input() override;

        virtual bool blocks_update() const override { return true; }
        virtual bool blocks_render() const override { return true; }
    protected:
        GameStateManager* _stateManager;
        registry _registry;
        DLLoader _systemLoader;

        bool _initialized{false};

        void set_state_manager(GameStateManager* manager) override { _stateManager = manager; }
};

inline std::string state_type_to_string(IGameState::GameStateType type) {
    switch (type) {
        case IGameState::GameStateType::MenusBackground: return "MenusBackground";
        case IGameState::GameStateType::MainMenu: return "MainMenu";
        case IGameState::GameStateType::Settings: return "Settings";
        case IGameState::GameStateType::SettingsPanel: return "SettingsPanel";
        case IGameState::GameStateType::InGame: return "InGame";
        case IGameState::GameStateType::InGameHud: return "InGameHud";
        case IGameState::GameStateType::InGameBackground: return "InGameBackground";

        case IGameState::GameStateType::Connection: return "Connection";
        case IGameState::GameStateType::Credits: return "Credits";
        case IGameState::GameStateType::Lobby: return "Lobby";
        case IGameState::GameStateType::AudioSettings: return "AudioSettings";
        case IGameState::GameStateType::VideoSettings: return "VideoSettings";
        case IGameState::GameStateType::BindingsSettings: return "BindsSettings";
        default: return "Unknown";
    }
}