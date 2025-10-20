#include "ServerECS.hpp"
#include <iostream>
#include "Utils/Console.hpp"
#include <unordered_map>
#include <chrono>
#include "ECS/Components/InputBuffer.hpp"
#include "Protocol/Protocol.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "Entity/Components/Controllable/Controllable.hpp"

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

        for (auto &pkt : packets) {
            if (pkt.data.empty()) continue;
            uint8_t msg_type = static_cast<uint8_t>(pkt.data[0]);
            std::vector<char> payload;
            if (pkt.data.size() > 1) payload.insert(payload.end(), pkt.data.begin() + 1, pkt.data.end());

            std::cout << Console::blue("[ServerECS] ") << "Pkt type=" << int(msg_type) << " from " << pkt.session_id
                    << " payload=" << payload.size() << std::endl;

            // Handle connect messages specially
            if (msg_type == static_cast<uint8_t>(RType::Protocol::SystemMessage::CLIENT_CONNECT)) {
                // Parse client connect structure if present
                const RType::Protocol::ClientConnect* cc = nullptr;
                if (!payload.empty() && payload.size() >= sizeof(RType::Protocol::ClientConnect)) {
                    cc = reinterpret_cast<const RType::Protocol::ClientConnect*>(payload.data());
                }

                // Create a new player entity and add base components
                entity player_ent = registry_.spawn_entity();
                // Default spawn position
                float spawn_x = 100.0f;
                float spawn_y = 100.0f;
                factory_->create_component<position>(registry_, player_ent, spawn_x, spawn_y);
                factory_->create_component<velocity>(registry_, player_ent, 0.0f, 0.0f);
                factory_->create_component<controllable>(registry_, player_ent, 200.0f);
                factory_->create_component<InputBuffer>(registry_, player_ent);

                // Store mapping
                session_entity_map_[pkt.session_id] = player_ent;
                // Allocate and store a numeric session token and return it to the client
                uint32_t token = next_session_token_++;
                session_token_map_[pkt.session_id] = token;

                // Send ServerAccept back if callback available (include numeric session token)
                if (send_callback_) {
                    RType::Protocol::ServerAccept sa;
                    sa.player_id = static_cast<uint32_t>(player_ent);
                    sa.session_id = token; // return the allocated numeric session token
                    sa.spawn_x = spawn_x;
                    sa.spawn_y = spawn_y;
                    auto packet = RType::Protocol::create_packet(static_cast<uint8_t>(RType::Protocol::SystemMessage::SERVER_ACCEPT), sa, RType::Protocol::PacketFlags::RELIABLE);
                    send_callback_(pkt.session_id, packet);
                }

                continue; // processed this packet
            }

            // Route input to an existing session entity's InputBuffer
            auto it = session_entity_map_.find(pkt.session_id);
            if (it != session_entity_map_.end()) {
                entity player_ent = it->second;
                // Ensure InputBuffer component registered
                registry_.register_component<InputBuffer>();

                // Attach or append to InputBuffer on player entity
                auto* buffers = registry_.get_if<InputBuffer>();
                if (buffers && buffers->has(player_ent)) {
                    auto& buf = (*buffers)[player_ent];
                    Input input{msg_type, std::move(payload), static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now().time_since_epoch()).count())};
                    buf.inputs.push_back(std::move(input));
                } else {
                    // Create InputBuffer for this entity
                    InputBuffer ib;
                    Input input{msg_type, std::move(payload), static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now().time_since_epoch()).count())};
                    ib.inputs.push_back(std::move(input));
                    factory_->create_component<InputBuffer>(registry_, player_ent, std::move(ib));
                }
            } else {
                // No player entity mapped yet - handle as lobby/connect packet or create a placeholder
                std::cout << Console::yellow("[ServerECS] ") << "No mapped entity for session " << pkt.session_id << ". Packet ignored or handled elsewhere." << std::endl;
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
