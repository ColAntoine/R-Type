#pragma once

class IGameCore {
    public:
        virtual ~IGameCore() = default;

        virtual bool init() = 0;
        virtual bool start() = 0;
        virtual bool update(float deltaTime) = 0;
        virtual bool shutdown() = 0;

    protected:
        IGameCore() = default;
        IGameCore(const IGameCore&) = delete;
        IGameCore& operator=(const IGameCore&) = delete;
        IGameCore(IGameCore&&) = delete;
        IGameCore& operator=(IGameCore&&) = delete;
};
