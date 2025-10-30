#include "ServerECS.hpp"
#include "Communication/Multiplayer.hpp"
#include <iostream>
#include "ECS/Utils/Console.hpp"
#include "Entity/Components/Enemy/Enemy.hpp"
#include "ECS/Components/Position.hpp"
#include <algorithm>

#include "Network/UDPServer.hpp"

namespace RType::Network {

    ServerECS::ServerECS(int maxLobbies) : max_lobbies_(maxLobbies) {
        multiplayer_ = std::make_unique<Multiplayer>(*this, maxLobbies);
    }
    ServerECS::~ServerECS() = default;

    bool ServerECS::init(const std::string& components_so) {
        if (!loader_.load_components_from_so(components_so, registry_)) {
            std::cerr << "Warning: failed to load ECS components from " << components_so << std::endl;
        }
        factory_ = loader_.get_factory();
        if (factory_) {
            std::cout << Console::green("[ServerECS] ") << "Component factory available" << std::endl;
        } else {
            std::cerr << Console::red("[ServerECS] ") << "Component factory not available after load" << std::endl;
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
                std::cout << Console::blue("[ServerECS] ") << "Processing pkt from " << pkt.session_id << " on port=" << pkt.server_port << " size=" << pkt.data.size() << std::endl;
                multiplayer_->handle_packet(pkt.session_id, pkt.data);
            }
        }
    }

    void ServerECS::tick(float dt) {
        // Track enemy count before update
        size_t enemy_count_before = 0;
        std::vector<entity> enemies_before;
        try {
            auto& registry = GetRegistry();
            registry.register_component<Enemy>();
            auto* enemies = registry.get_if<Enemy>();
            if (enemies) {
                enemy_count_before = enemies->size();
                for (size_t i = 0; i < enemies->size(); ++i) {
                    if (enemies->has(i)) {
                        enemies_before.push_back(entity(i));
                    }
                }
            }
        } catch (...) {}
        
        // run all registered systems (systems may consume net_input components)
        loader_.update_all_systems(registry_, dt, DLLoader::LogicSystem);

        // After position system has run, broadcast updated positions to all clients
        if (multiplayer_) {
            multiplayer_->broadcast_positions();
        }
        
        // Check for new enemies and broadcast them
        try {
            auto& registry = GetRegistry();
            auto* enemies = registry.get_if<Enemy>();
            auto* positions = registry.get_if<position>();
            
            if (enemies && positions && multiplayer_) {
                for (size_t i = 0; i < enemies->size(); ++i) {
                    if (enemies->has(i)) {
                        entity ent(i);
                        // Check if this is a new enemy (wasn't in the before list)
                        bool is_new = std::find(enemies_before.begin(), enemies_before.end(), ent) == enemies_before.end();
                        
                        if (is_new && positions->has(i)) {
                            // Get enemy data
                            const auto& enemy_comp = (*enemies)[ent];
                            const auto& pos = (*positions)[ent];
                            uint8_t enemy_type = static_cast<uint8_t>(enemy_comp.enemy_type);
                            
                            // Broadcast to clients
                            multiplayer_->broadcast_enemy_spawn(ent, enemy_type, pos.x, pos.y);
                        }
                    }
                }
            }
        } catch (...) {
            std::cerr << Console::red("[ServerECS] ") << "Exception occurred during enemy broadcasting (details unavailable)" << std::endl;
        }
    }

    registry& ServerECS::GetRegistry() {
        return registry_;
    }

} // namespace RType::Network
