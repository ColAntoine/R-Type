#include "unknown_handler.hpp"
#include "session.hpp"
#include <iostream>
#include <iomanip>

namespace RType::Network {

bool UnknownMessageHandler::handle_message(std::shared_ptr<Session> session, const char* data, size_t size) {
    if (size == 0) {
        std::cerr << "Received empty message from session: " << session->get_session_id() << std::endl;
        return false;
    }

    uint8_t message_type = static_cast<uint8_t>(data[0]);

    std::cout << "Unknown message type 0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2)
              << static_cast<int>(message_type) << std::dec
              << " from session: " << session->get_session_id()
              << " (size: " << size << " bytes)" << std::endl;

    // Print first few bytes for debugging
    std::cout << "Data: ";
    for (size_t i = 0; i < std::min(size, static_cast<size_t>(16)); ++i) {
        std::cout << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2)
                  << static_cast<int>(static_cast<uint8_t>(data[i])) << " ";
    }
    std::cout << std::dec << std::endl;

    return false; // Unknown message types are not handled
}

} // namespace RType::Network