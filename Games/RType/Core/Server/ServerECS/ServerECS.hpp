#pragma once

#include "Protocol/MessageQueue.hpp"
#include "ECS/DLLoader.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components/NetInput.hpp"
#include "Core/AGameCore.hpp"
#include <string>
#include <chrono>
#include <unordered_map>

namespace RType::Network {

class ServerECS {
    public:
        ServerECS();
        ~ServerECS();

        // Initialize components/systems from shared object (optional)
        bool init(const std::string& components_so = "lib/libECS.so");

        // Attach the network message queue used by the UDP server
        void set_message_queue(MessageQueue* q);

        // Process all pending network packets (drain queue and convert to net_input components)
        void process_packets();

        // Run ECS systems for one tick (dt in seconds)
        void tick(float dt);

        // Access registry for advanced ops/tests
        registry& GetRegistry();

    private:
        DLLoader loader_;
        registry registry_;
        IComponentFactory* factory_{nullptr};
        MessageQueue* msgq_{nullptr};

        // Map player_id to entity
        std::unordered_map<uint32_t, entity> player_entities_;
};

} // namespace RType::Network
