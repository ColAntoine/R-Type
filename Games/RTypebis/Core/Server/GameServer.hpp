#pragma once

#include "../IGameCore.hpp"

class GameServer : public IGameCore {
    public:
        GameServer() = default;
        virtual ~GameServer() override = default;

        virtual void init() override;
        virtual void update(float deltaTime) override;
        virtual void shutdown() override;
    private:
};
