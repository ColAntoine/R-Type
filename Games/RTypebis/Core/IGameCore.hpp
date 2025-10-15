#pragma once

class IGameCore {
    public:
        virtual ~IGameCore() = default;
        virtual void init() = 0;
        virtual void update(float deltaTime) = 0;
        virtual void shutdown() = 0;

    protected:
        IGameCore() = default;
        IGameCore(const IGameCore&) = delete;
        IGameCore& operator=(const IGameCore&) = delete;
        IGameCore(IGameCore&&) = delete;
        IGameCore& operator=(IGameCore&&) = delete;
};
