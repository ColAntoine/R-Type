#include "ServerECS.hpp"
#include "Core/Server/Protocol/Protocol.hpp"
#include "ECS/Components/Position.hpp"
#include <iostream>
#include <cstring>
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

        // Ensure Position component is registered
        registry_.register_component<position>();

        for (auto &pkt : packets) {
            // ReceivedPacket.data format: [message_type (1 byte), payload...]
            if (pkt.data.size() < 1) continue;
            
            // First byte is the message type
            uint8_t message_type = static_cast<uint8_t>(pkt.data[0]);
            
            // Payload starts at byte 1
            const char* payload = pkt.data.data() + 1;
            size_t payload_size = pkt.data.size() - 1;

            std::cout << Console::blue("[ServerECS] ") << "Pkt type=" << int(message_type) 
                      << " size=" << payload_size << " from session=" << pkt.session_id << std::endl;

            // Handle POSITION_UPDATE messages
            if (message_type == static_cast<uint8_t>(Protocol::GameMessage::POSITION_UPDATE)) {
                if (payload_size >= sizeof(Protocol::PositionUpdate)) {
                    Protocol::PositionUpdate pos_update;
                    std::memcpy(&pos_update, payload, sizeof(pos_update));

                    uint32_t player_id = pos_update.entity_id;
                    float px = pos_update.x;  // Copy to avoid packed field binding issue
                    float py = pos_update.y;
                    
                    // Find or create entity for this player
                    if (player_entities_.find(player_id) == player_entities_.end()) {
                        // Create new player entity
                        auto e = registry_.spawn_entity();
                        player_entities_[player_id] = e;
                        registry_.emplace_component<position>(e, px, py);
                        std::cout << Console::green("[ServerECS] ") << "Created new player entity for ID " 
                                  << player_id << " at (" << px << ", " << py << ")" << std::endl;
                    } else {
                        // Update existing player position
                        auto e = player_entities_[player_id];
                        auto* pos_array = registry_.get_if<position>();
                        if (pos_array && pos_array->has(static_cast<size_t>(e))) {
                            (*pos_array)[static_cast<size_t>(e)].x = px;
                            (*pos_array)[static_cast<size_t>(e)].y = py;
                        }
                    }
                }
            }
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
