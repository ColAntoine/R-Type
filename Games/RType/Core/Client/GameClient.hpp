#pragma once

#include "../AGameCore.hpp"
#include "ECS/Registry.hpp"

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
};
