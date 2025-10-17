#pragma once

#include "Core/AGameCore.hpp"
#include "SceneManager.hpp"
#include "ECS/Entity.hpp"

class GameClient : public AGameCore {
    public:
        GameClient() = default;
        virtual ~GameClient() override = default;

        virtual bool init() override;
        virtual bool start() override;
        virtual bool update(float deltaTime) override;
        virtual bool shutdown() override;

        static IGameCore& getInstance();
    private:
        entity _playerEntity = 0;
};
