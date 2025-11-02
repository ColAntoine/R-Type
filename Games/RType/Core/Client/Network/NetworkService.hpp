// Service locator for client-side NetworkManager
#pragma once

#include <memory>
#include "NetworkManager.hpp"

namespace RType::Network {

    void set_network_manager(NetworkManager* manager);

    NetworkManager* get_network_manager();

} // namespace RType::Network
