#pragma once

#include <cstdint>
#include <cstring>

namespace Protocol {

    // Packet Header (4 bytes)
    struct PacketHeader {
        uint8_t  message_type;
        uint8_t  flags;
        uint16_t payload_size;
    } __attribute__((packed));

    enum class PacketFlags : uint8_t {
        NONE = 0x00,
    };

    enum class SystemMessage : uint8_t {
        SERVER_ACCEPT = 0x02,
        PLAYER_DISCONNECT = 0x05,
    };

    enum class RTypeMessage : uint8_t {
        ENTITY_UPDATE   = 0xC1,
        ENTITY_DESTROY  = 0xC2,
        POSITION_UPDATE = 0xC7,
    };

    struct ServerAccept {
        uint32_t player_id;
        uint32_t server_version;
        uint8_t server_capabilities;
    } __attribute__((packed));

    struct PlayerDisconnect {
        uint32_t player_id;
    } __attribute__((packed));

    struct PositionUpdate {
        uint32_t player_id;
        float position_x;
        float position_y;
    } __attribute__((packed));

    struct EntityUpdate {
        uint32_t entity_id;
        uint32_t timestamp;
        float position_x;
        float position_y;
        float velocity_x;
        float velocity_y;
        uint8_t state_flags;
    } __attribute__((packed));

    struct EntityDestroy {
        uint32_t entity_id;
        uint8_t entity_type; // 0 = player, 1 = enemy
    } __attribute__((packed));

    inline size_t create_packet(uint8_t* buffer, uint8_t message_type, const void* payload, uint16_t payload_size) {
        PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
        header->message_type = message_type;
        header->flags = static_cast<uint8_t>(PacketFlags::NONE);
        header->payload_size = payload_size;
        if (payload && payload_size > 0) {
            memcpy(buffer + sizeof(PacketHeader), payload, payload_size);
        }
        return sizeof(PacketHeader) + payload_size;
    }

    inline bool parse_packet(const uint8_t* buffer, size_t buffer_size, PacketHeader& header, const uint8_t*& payload) {
        if (buffer_size < sizeof(PacketHeader)) {
            return false;
        }
        header = *reinterpret_cast<const PacketHeader*>(buffer);
        if (buffer_size < sizeof(PacketHeader) + header.payload_size) {
            return false;
        }
        payload = buffer + sizeof(PacketHeader);
        return true;
    }

} // namespace Protocol