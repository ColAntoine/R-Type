#pragma once

#include "../AGameCore.hpp"
#include "ECS/Registry.hpp"
#include "Core/States/GameStateManager.hpp"
#include <raylib.h>

class GameClient : public AGameCore {
    public:
        GameClient();
        ~GameClient();

        bool init() override;
        void run() override;
        void update(float delta) override;
        void shutdown() override;

    private:
        GameStateManager _stateManager;

        void register_states();
};
