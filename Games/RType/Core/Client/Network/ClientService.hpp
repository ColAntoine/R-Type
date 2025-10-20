// Small service locator for client-side UdpClient
#pragma once

#include <memory>
#include "UDPClient.hpp"

namespace RType::Network {

// Set the global/shared UdpClient instance
    void set_client(std::shared_ptr<UdpClient> client);

    // Get the global/shared UdpClient instance (may return nullptr if not set)
    std::shared_ptr<UdpClient> get_client();

} // namespace RType::Network
