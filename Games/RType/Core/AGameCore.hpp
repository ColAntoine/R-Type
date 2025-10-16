#pragma once

#include "IGameCore.hpp"

class AGameCore : public IGameCore {
    public:
        AGameCore();
        virtual ~AGameCore();

        // Default lifecycle implementations forward to overridable hooks
        bool init() override;
        void run() override;
        void update(float) override;
        void shutdown() override;

    protected:
        // Hooks for derived classes
        virtual bool onInit() { return true; }
        virtual void onRun() {}
        virtual void onUpdate(float) {}
        virtual void onShutdown() {}
};
