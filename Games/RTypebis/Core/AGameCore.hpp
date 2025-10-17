#pragma once

#include "IGameCore.hpp"
#include "ECS/DLLoader.hpp"
#include "ECS/Registry.hpp"

class AGameCore : public IGameCore {
    public:
        virtual ~AGameCore() override = default;

        virtual bool init() override = 0;
        virtual bool start() override = 0;
        virtual bool update(float deltaTime) override = 0;
        virtual bool shutdown() override = 0;

    protected:
        AGameCore() = default;
        AGameCore(const AGameCore&) = delete;
        AGameCore& operator=(const AGameCore&) = delete;
        AGameCore(AGameCore&&) = delete;
        AGameCore& operator=(AGameCore&&) = delete;

        DLLoader _ecsLoader = 0;
        registry _ecsRegistry = 0;
        IComponentFactory* _componentFactory = nullptr;

        SceneManager _sceneManager = 0;

        bool _isRunning = false;
};
