#pragma once

#include "Core/States/AGameState.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Components.hpp"

#include "Entity/Components/Controllable/Controllable.hpp"
#include "Entity/Components/Weapon/Weapon.hpp"
#include "Entity/Components/Score/Score.hpp"
#include "Entity/Components/Health/Health.hpp"

#include "Constants.hpp"

class InGameState : public AGameState {
    public:
        InGameState();
        ~InGameState() override = default;

        void enter() override;
        void exit() override;
        void pause() override;
        void resume() override;

        void setup_ui() override;

        void update(float delta_time) override;

        std::string get_name() const override { return "InGame"; }
    private:
        // InGame specific members
        void createPlayer();
        void handle_input();

        entity _playerEntity;

        // Active references (resolved once in enter())
        registry* active_registry_{nullptr};
        DLLoader* active_loader_{nullptr};
};