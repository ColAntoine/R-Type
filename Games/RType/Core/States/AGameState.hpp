#pragma once

#include "IGameState.hpp"
#include "ECS/ILoader.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Systems/UISystem.hpp"
#include "ECS/Components/UIComponent.hpp"
#include "ECS/UI/Components/Button.hpp"
#include "ECS/UI/Components/Panel.hpp"
#include "ECS/UI/Components/Text.hpp"


#include "Core/States/GameStateManager.hpp"

namespace EventTypes {
    const std::string SCORE_INCREASED = "SCORE_INCREASED";
}
class AGameState : public IGameState {
    public:
        enum MoveDirection {
            Left,
            Right
        };

        AGameState();
        AGameState(registry* shared_registry, ILoader* shared_loader);
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
        std::unique_ptr<ILoader> _systemLoader;
        
        // Optional pointers to shared registry/loader (for states that need to share ECS)
        registry* _shared_registry{nullptr};
        ILoader* _shared_loader{nullptr};

        bool _initialized{false};

        EventBus::CallbackId _uiEventCallbackId; // Used to unsubscribe

        void set_state_manager(GameStateManager* manager) override { _stateManager = manager; }
        void subscribe_to_ui_event();
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
        case IGameState::GameStateType::InGamePause: return "InGamePause";
        case IGameState::GameStateType::Connection: return "Connection";
        case IGameState::GameStateType::Credits: return "Credits";
        case IGameState::GameStateType::Lobby: return "Lobby";
        case IGameState::GameStateType::AudioSettings: return "AudioSettings";
        case IGameState::GameStateType::VideoSettings: return "VideoSettings";
        case IGameState::GameStateType::BindingsSettings: return "BindingsSettings";
        case IGameState::GameStateType::LoadingVideo: return "LoadingVideo";
        case IGameState::GameStateType::InGameExit: return "InGameExit";
        default: return "Unknown";
    }
}
