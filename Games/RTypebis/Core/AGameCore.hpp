#pragma once

#include "IGameCore.hpp"

class AGameCore : public IGameCore {
    public:
        virtual ~AGameCore() override = default;

        virtual void init() override = 0;
        virtual void update(float deltaTime) override = 0;
        virtual void shutdown() override = 0;

    protected:
        AGameCore() = default;
        AGameCore(const AGameCore&) = delete;
        AGameCore& operator=(const AGameCore&) = delete;
        AGameCore(AGameCore&&) = delete;
        AGameCore& operator=(AGameCore&&) = delete;
};
