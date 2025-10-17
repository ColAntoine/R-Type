// Minimal IGameCore interface with lifecycle methods
#pragma once

class IGameCore {
    public:
        virtual ~IGameCore() = default;

        // Initialize resources. Return true on success.
        virtual bool init() = 0;

        // Main loop entry (blocking or non-blocking depending on implementation)
        virtual void run() = 0;

        // Single-step update called by run loop when appropriate
        virtual void update(float delta) = 0;

        // Cleanup resources
        virtual void shutdown() = 0;
};
