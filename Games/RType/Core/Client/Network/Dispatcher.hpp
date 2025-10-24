#pragma once

#include <functional>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <shared_mutex>
#include <mutex>

// Simple thread-safe dispatcher for incoming network messages on the client
class NetworkDispatcher {
    public:
        using Handler = std::function<void(const char*, size_t)>;

        // Register a handler for a specific message type (overwrites existing)
        void register_handler(uint8_t message_type, Handler h) {
            std::unique_lock lock(mutex_);
            handlers_[message_type] = std::move(h);
        }

        // Dispatch payload for a message type (non-throwing)
        void dispatch(uint8_t message_type, const char* payload, size_t size) const {
            std::shared_lock lock(mutex_);
            auto it = handlers_.find(message_type);
            if (it != handlers_.end()) {
                try {
                    it->second(payload, size);
                } catch (...) {
                    // swallow handler exceptions to avoid bringing down receiver thread
                }
            }
        }

    private:
        mutable std::shared_mutex mutex_;
        std::unordered_map<uint8_t, Handler> handlers_;
};
