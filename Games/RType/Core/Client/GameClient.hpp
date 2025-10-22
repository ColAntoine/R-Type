#pragma once

#include "../AGameCore.hpp"
#include "ECS/Registry.hpp"
#include "Core/States/GameStateManager.hpp"
#include "Core/States/NetworkState.hpp"
#include <raylib.h>
#include <memory>

class GameClient : public AGameCore {
    public:
        GameClient();
        ~GameClient();

        bool init() override;
        void run() override;
        void update(float delta) override;
        void shutdown() override;

        registry& GetRegistry();

    private:
        registry ecs_registry_;
        GameStateManager state_manager_;
        std::shared_ptr<NetworkState> network_state_;
        bool running_{false};

        void register_states();
};
