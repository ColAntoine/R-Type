#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

#include "Entity/Components/Controllable/Controllable.hpp"
#include "Entity/Components/Weapon/Weapon.hpp"
#include "Entity/Components/Score/Score.hpp"
#include "Entity/Components/Health/Health.hpp"
#include "Entity/Components/CurrentWave/CurrentWave.hpp"

#include "Constants.hpp"

class InGameState : public AGameState {
    public:
        InGameState() = default;
        InGameState(registry* shared_registry, ILoader* shared_loader)
            : AGameState(shared_registry, shared_loader) {}
        ~InGameState() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;

        void update(float delta_time) override;

    void handle_input();

        std::string get_name() const override { return "InGame"; }
    private:
        // InGame specific members
        void createPlayer();
        void startMusic();

        entity _playerEntity;
        EventBus::CallbackId _keyPressedCallbackId;  // Callback ID for key pressed event

        // Fixed timestep for deterministic physics (match server at 30Hz)
        static constexpr float FIXED_DT = 0.033f;  // 33ms = ~30 FPS
        float _accumulator = 0.0f;
};

