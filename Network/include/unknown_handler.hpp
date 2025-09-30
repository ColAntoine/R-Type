#pragma once

#include "message_handler.hpp"

namespace RType::Network {

/**
 * @brief Fallback handler for unknown message types
 */
class UnknownMessageHandler : public IMessageHandler {
public:
    uint8_t get_message_type() const override {
        // This will never be matched directly, used as fallback
        return 0xFF;
    }

    bool handle_message(std::shared_ptr<Session> session, const char* data, size_t size) override;
};

} // namespace RType::Network