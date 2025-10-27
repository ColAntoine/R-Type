// Service locator for client-side NetworkManager
#pragma once

#include <memory>
#include "NetworkManager.hpp"

namespace RType::Network {

    // Set the global/shared NetworkManager instance
    void set_network_manager(NetworkManager* manager);

    // Get the global/shared NetworkManager instance (may return nullptr if not set)
    NetworkManager* get_network_manager();

} // namespace RType::Network
