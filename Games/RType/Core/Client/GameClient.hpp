#pragma once

#include "../AGameCore.hpp"
#include "ECS/Registry.hpp"
#include "ECS/DLLoader.hpp"
#include "Core/States/GameStateManager.hpp"
#include <raylib.h>
#include "Network/NetworkManager.hpp"

class GameClient : public AGameCore {
    public:
        GameClient(float scale = 1.0f, bool windowed = false);
        ~GameClient();

        bool init() override;
        void run() override;
        void update(float delta) override;
        void shutdown() override;

        registry &GetRegistry();
        DLLoader &GetDLLoader();

    private:
        registry ecs_registry_;
        DLLoader ecs_loader_;
        std::unique_ptr<NetworkManager> network_manager_;

        GameStateManager _stateManager;
        float _scale;
        bool _windowed;
        void register_states();
};
