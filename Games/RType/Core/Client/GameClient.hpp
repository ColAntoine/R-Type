#pragma once

#include "../AGameCore.hpp"
#include "ECS/Registry.hpp"
#include "ECS/DLLoader.hpp"
#include "Core/States/GameStateManager.hpp"
#include <raylib.h>
#include "Network/NetworkManager.hpp"

class GameClient : public AGameCore {
    public:
        GameClient();
        ~GameClient();

        bool init() override;
        void run() override;
        void update(float delta) override;
        void shutdown() override;

        registry &GetRegistry();
        DLLoader &GetDLLoader();

        void load_systems();

    private:
        registry ecs_registry_;
        DLLoader ecs_loader_;
        std::unique_ptr<NetworkManager> network_manager_;
        GameStateManager state_manager_;
        bool running_{false};

        void register_states();
};
