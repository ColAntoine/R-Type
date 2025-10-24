#include "ClientService.hpp"

namespace RType::Network {

    static std::shared_ptr<UdpClient> g_client = nullptr;

    void set_client(std::shared_ptr<UdpClient> client) {
        g_client = std::move(client);
    }

    std::shared_ptr<UdpClient> get_client() {
        return g_client;
    }

} // namespace RType::Network
