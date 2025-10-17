#include "ServerECS.hpp"
#include <iostream>
#include "Utils/Console.hpp"

namespace RType::Network {

    ServerECS::ServerECS() = default;
    ServerECS::~ServerECS() = default;

    bool ServerECS::init(const std::string& components_so) {
        if (!loader_.load_components_from_so(components_so, registry_)) {
            std::cerr << "Warning: failed to load ECS components from " << components_so << std::endl;
        }
        factory_ = loader_.get_factory();
        if (!factory_) std::cerr << "Warning: no component factory available from DLLoader" << std::endl;
        return true;
    }

    void ServerECS::set_message_queue(MessageQueue* q) {
        msgq_ = q;
    }

    void ServerECS::process_packets() {
        if (!msgq_) return;
        std::vector<ReceivedPacket> packets;
        msgq_->drain(packets);

        // Ensure NetInput component is registered
        registry_.register_component<NetInput>();

        for (auto &pkt : packets) {
            if (pkt.data.empty()) continue;
            uint8_t msg_type = static_cast<uint8_t>(pkt.data[0]);
            std::vector<char> payload;
            if (pkt.data.size() > 1) payload.insert(payload.end(), pkt.data.begin() + 1, pkt.data.end());

            std::cout << Console::blue("[ServerECS] ") << "Pkt type=" << int(msg_type) << " from " << pkt.session_id
                    << " payload=" << payload.size() << std::endl;

            // Create ephemeral entity and attach net_input. Real mapping will be added later.
            auto e = registry_.spawn_entity();
            registry_.emplace_component<NetInput>(e, msg_type, std::move(payload), pkt.session_id);
        }
    }

    void ServerECS::tick(float dt) {
        // run all registered systems (systems may consume net_input components)
        loader_.update_all_systems(registry_, dt);
    }

    registry& ServerECS::GetRegistry() {
        return registry_;
    }

} // namespace RType::Network
