#include "ServerECS.hpp"
#include "Communication/Multiplayer.hpp"
#include <iostream>
#include "Entity/Components/Enemy/Enemy.hpp"
#include "ECS/Components/Position.hpp"
#include <algorithm>

#include "Network/UDPServer.hpp"

#ifdef _WIN32
    #include "ECS/WinLoader.hpp"
    using PlatformLoader = WinLoader;
#else
    #include "ECS/LinuxLoader.hpp"
    using PlatformLoader = LinuxLoader;
#endif

namespace RType::Network {

    ServerECS::ServerECS(int maxLobbies, int maxPlayers) : max_lobbies_(maxLobbies), max_players_(maxPlayers) {
        loader_ = std::make_unique<PlatformLoader>();
        multiplayer_ = std::make_unique<Multiplayer>(*this, maxLobbies, maxPlayers);
    }
    ServerECS::~ServerECS() = default;

    bool ServerECS::init(const std::string& components_so) {
        if (!loader_->load_components(components_so, registry_)) {
            std::cerr << "Warning: failed to load ECS components from " << components_so << std::endl;
        }
        factory_ = loader_->get_factory();
        if (factory_) {
            std::cout << "Component factory available" << std::endl;
        } else {
            std::cerr << "Component factory not available after load" << std::endl;
        }
        return true;
    }

    void ServerECS::set_message_queue(MessageQueue* q) {
        msgq_ = q;
    }

    void ServerECS::set_udp_server(UdpServer* server) {
        if (multiplayer_) {
            multiplayer_->set_udp_server(server);
        }
    }

    void ServerECS::process_packets() {
        if (!msgq_) return;
        std::vector<ReceivedPacket> packets;
        msgq_->drain(packets);
        for (auto &pkt : packets) {
            if (pkt.data.empty()) continue;
            if (multiplayer_) {
                std::cout << "[ServerECS] Processing pkt from " << pkt.session_id << " on port=" << pkt.server_port << " size=" << pkt.data.size() << std::endl;
                multiplayer_->handle_packet(pkt.session_id, pkt.data);
            }
        }
    }

    void ServerECS::tick(float dt) {
        loader_->update_all_systems(registry_, dt, ILoader::LogicSystem);

        if (multiplayer_) {
            multiplayer_->broadcast_loop();
        }
    }

    registry& ServerECS::GetRegistry() {
        return registry_;
    }

} // namespace RType::Network
