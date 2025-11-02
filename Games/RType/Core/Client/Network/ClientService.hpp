// Small service locator for client-side UdpClient
#pragma once

#include <memory>
#include "UDPClient.hpp"

namespace RType::Network {

    void set_client(std::shared_ptr<UdpClient> client);

    std::shared_ptr<UdpClient> get_client();

} // namespace RType::Network
