#include "message_handler.hpp"
#include "session.hpp"
#include <iostream>
#include <iomanip>

namespace RType::Network {

void MessageDispatcher::register_handler(uint8_t message_type, handler_ptr handler) {
    handlers_[message_type] = handler;
}

void MessageDispatcher::unregister_handler(uint8_t message_type) {
    handlers_.erase(message_type);
}

bool MessageDispatcher::dispatch_message(std::shared_ptr<Session> session, const char* data, size_t size) {
    if (size == 0) {
        return false;
    }

    uint8_t message_type = static_cast<uint8_t>(data[0]);

    auto it = handlers_.find(message_type);
    if (it == handlers_.end()) {
        // Print detailed debug info for unknown messages
        std::cout << "No handler registered for message type: 0x"
                  << std::hex << std::uppercase << std::setfill('0') << std::setw(2)
                  << static_cast<int>(message_type) << std::dec
                  << " from session: " << session->get_session_id()
                  << " (size: " << size << " bytes)" << std::endl;

        // Print first few bytes for debugging
        std::cout << "Raw data: ";
        for (size_t i = 0; i < std::min(size, size_t(16)); ++i) {
            std::cout << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2)
                      << static_cast<int>(static_cast<uint8_t>(data[i])) << " ";
        }
        std::cout << std::dec << std::endl;

        return false;
    }

    try {
        return it->second->handle_message(session, data + 1, size - 1);
    } catch (const std::exception& e) {
        std::cerr << "Exception in message handler for type 0x"
                  << std::hex << static_cast<int>(message_type) << std::dec
                  << ": " << e.what() << std::endl;
        return false;
    }
}

bool MessageDispatcher::has_handler(uint8_t message_type) const {
    return handlers_.find(message_type) != handlers_.end();
}

} // namespace RType::Network