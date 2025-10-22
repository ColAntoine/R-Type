#include "ServerECS.hpp"
#include "Core/Server/Network/UDPServer.hpp"
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

    void ServerECS::set_udp_server(UdpServer* server) {
        udp_server_ = server;
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
            // Handle CLIENT_READY messages
            else if (message_type == static_cast<uint8_t>(Protocol::SystemMessage::CLIENT_READY)) {
                if (payload_size >= sizeof(Protocol::ClientReady)) {
                    Protocol::ClientReady ready_msg;
                    std::memcpy(&ready_msg, payload, sizeof(ready_msg));
                    
                    set_player_ready(ready_msg.player_id, ready_msg.ready_state != 0);
                }
            }
            // Handle CLIENT_CONNECT messages (for lobby management)
            else if (message_type == static_cast<uint8_t>(Protocol::SystemMessage::CLIENT_CONNECT)) {
                if (payload_size >= sizeof(Protocol::ClientConnect)) {
                    Protocol::ClientConnect connect_msg;
                    std::memcpy(&connect_msg, payload, sizeof(connect_msg));
                    
                    // Note: We don't have the assigned player_id here since it's assigned by UDPServer
                    // We'll need to get it from somewhere else. For now, let's assume it's in the session
                    // This is a limitation - we need to modify how this works.
                    
                    // For now, let's skip this and handle player addition differently
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

    // Lobby management methods
    void ServerECS::add_player(uint32_t player_id, const std::string& name) {
        // Check if player already exists
        for (auto& player : players_) {
            if (player.player_id == player_id) {
                player.is_connected = true;
                player.name = name;
                return;
            }
        }

        // Add new player
        PlayerInfo new_player;
        new_player.player_id = player_id;
        new_player.name = name;
        new_player.is_ready = false;
        new_player.is_connected = true;
        players_.push_back(new_player);

        std::cout << Console::green("[ServerECS] ") << "Added player " << name << " (ID: " << player_id << ") to lobby" << std::endl;
        broadcast_player_list();
    }

    void ServerECS::remove_player(uint32_t player_id) {
        for (auto it = players_.begin(); it != players_.end(); ++it) {
            if (it->player_id == player_id) {
                std::cout << Console::yellow("[ServerECS] ") << "Removed player " << it->name << " (ID: " << player_id << ") from lobby" << std::endl;
                players_.erase(it);
                broadcast_player_list();
                return;
            }
        }
    }

    void ServerECS::set_player_ready(uint32_t player_id, bool ready) {
        for (auto& player : players_) {
            if (player.player_id == player_id) {
                player.is_ready = ready;
                std::cout << Console::blue("[ServerECS] ") << "Player " << player.name << " (ID: " << player_id << ") is now " 
                          << (ready ? "ready" : "not ready") << std::endl;
                broadcast_player_list();
                start_game_if_ready();
                return;
            }
        }
    }

    bool ServerECS::are_all_players_ready() const {
        if (players_.empty()) return false;

        for (const auto& player : players_) {
            if (!player.is_ready) return false;
        }
        return true;
    }

    void ServerECS::broadcast_player_list() {
        if (!udp_server_) return;

        Protocol::ClientListUpdate update_msg;
        update_msg.player_count = std::min(static_cast<size_t>(8), players_.size());

        for (size_t i = 0; i < update_msg.player_count; ++i) {
            const auto& player = players_[i];
            update_msg.players[i].player_id = player.player_id;
            update_msg.players[i].ready_state = player.is_ready ? 1 : 0;
            std::strncpy(update_msg.players[i].name, player.name.c_str(), sizeof(update_msg.players[i].name) - 1);
            update_msg.players[i].name[sizeof(update_msg.players[i].name) - 1] = '\0';
        }

        // Create buffer with message type + payload
        std::vector<char> buffer(sizeof(uint8_t) + sizeof(update_msg));
        buffer[0] = static_cast<uint8_t>(Protocol::SystemMessage::CLIENT_LIST);
        std::memcpy(buffer.data() + 1, &update_msg, sizeof(update_msg));

        // Broadcast to all connected clients
        udp_server_->broadcast(buffer.data(), buffer.size());
    }

    void ServerECS::start_game_if_ready() {
        if (game_started_ || !are_all_players_ready()) return;

        std::cout << Console::green("[ServerECS] ") << "All players ready! Starting game..." << std::endl;
        game_started_ = true;

        // Send start game message
        Protocol::StartGame start_msg;
        start_msg.timestamp = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());

        // Create buffer with message type + payload
        std::vector<char> buffer(sizeof(uint8_t) + sizeof(start_msg));
        buffer[0] = static_cast<uint8_t>(Protocol::SystemMessage::START_GAME);
        std::memcpy(buffer.data() + 1, &start_msg, sizeof(start_msg));

        udp_server_->broadcast(buffer.data(), buffer.size());
    }

} // namespace RType::Network
