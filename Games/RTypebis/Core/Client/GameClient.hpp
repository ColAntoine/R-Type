#pragma once

#include "../AGameCore.hpp"

class GameClient : public AGameCore {
    public:
        GameClient() = default;
        virtual ~GameClient() override = default;

        virtual void init() override;
        virtual void update(float deltaTime) override;
        virtual void shutdown() override;
    private:
};
