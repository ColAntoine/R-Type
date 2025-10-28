#include "NetworkService.hpp"

namespace RType::Network {

    static NetworkManager* g_network_manager = nullptr;

    void set_network_manager(NetworkManager* manager) {
        g_network_manager = manager;
    }

    NetworkManager* get_network_manager() {
        return g_network_manager;
    }
} // namespace RType::Network
