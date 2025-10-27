#pragma once

#include <cstdint>
#include <cstring>
#include <vector>
#include <memory>

namespace RType::Protocol {

    // ============================================================================
    // Protocol Constants
    // ============================================================================

    constexpr size_t MAX_PACKET_SIZE = 1024;
    constexpr size_t HEADER_SIZE = sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint16_t);

    // ============================================================================
    // Packet Structure
    // ============================================================================

    /**
     * @brief Standard packet header for all messages
     */
    struct PacketHeader {
        uint8_t  message_type;   ///< Type of message
        uint8_t  flags;          ///< Message flags
        uint16_t payload_size;   ///< Size of payload following header
    } __attribute__((packed));

    /**
     * @brief Packet flags
     */
    enum class PacketFlags : uint8_t {
        NONE = 0x00,
        RELIABLE = 0x01,    ///< Requires acknowledgment
        FRAGMENT = 0x02,    ///< Part of fragmented message
    };

    // ============================================================================
    // Message Types
    // ============================================================================

    /**
     * @brief System-level message types
     */
    enum class SystemMessage : uint8_t {
        // Connection management
        CLIENT_CONNECT    = 0x01,
        SERVER_ACCEPT     = 0x02,
        CLIENT_DISCONNECT = 0x03,
        SERVER_DISCONNECT = 0x04,
        CLIENT_READY      = 0x07,
        CLIENT_UNREADY    = 0x08,
        PING              = 0x05,
        PONG              = 0x06,

        // Server info
        SERVER_INFO       = 0x10,
        CLIENT_LIST       = 0x11,
        START_GAME        = 0x12,
    };

    /**
     * @brief Game-specific message types
     */
    enum class GameMessage : uint8_t {
        // Entity management
        ENTITY_CREATE     = 0xC0,
        PLAYER_SPAWN      = 0xC3,
        PLAYER_SPAWN_REMOTE = 0xC4,
        ENTITY_UPDATE     = 0xC1,
        ENTITY_DESTROY    = 0xC2,

        // Player actions
        PLAYER_JOIN       = 0xC5,
        PLAYER_LEAVE      = 0xC6,
        POSITION_UPDATE   = 0xC7,
        PLAYER_SHOOT      = 0xC8,

        // Game state
        GAME_START        = 0xD0,
        GAME_END          = 0xD1,
        SCORE_UPDATE      = 0xD2,
    };

    // ============================================================================
    // Message Structures
    // ============================================================================

    /**
     * @brief Client connection request
     */
    struct ClientConnect {
        char player_name[32];    ///< Player name (null-terminated)
        uint32_t client_version; ///< Client version
    } __attribute__((packed));

    /**
     * @brief Player spawn message (sent to the owning client)
     */
    struct PlayerSpawn {
        uint32_t player_token;   ///< Public session token assigned by server
        uint32_t server_entity;  ///< Server internal entity id (optional mapping)
        float x, y;              ///< Spawn position
        float health;            ///< Initial health
    } __attribute__((packed));

    /**
     * @brief Enemy spawn message (sent to other clients)
     */
    struct PlayerRemoteSpawn {
        uint32_t player_token;   ///< Public session token of the player represented
        uint32_t server_entity;  ///< Server internal entity id
        float x, y;              ///< Spawn position
        float health;            ///< Initial health
    } __attribute__((packed));

    /**
     * @brief Server acceptance response
     */
    struct ServerAccept {
        uint32_t player_id;      ///< Assigned player ID
        uint32_t session_id;     ///< Session identifier
        float spawn_x, spawn_y;  ///< Initial spawn position
    } __attribute__((packed));

    /**
     * @brief Client ready signal
     */
    struct ClientReady {
        uint32_t player_id;      ///< Player ID confirming readiness
        uint8_t ready_state;     ///< 1 = ready, 0 = not ready
    } __attribute__((packed));

    /**
     * @brief Client disconnect signal
     */
    struct ClientDisconnect {
        uint32_t player_id;      ///< Player ID disconnecting
        uint8_t reason;          ///< Disconnect reason (0=voluntary, 1=timeout, 2=error)
    } __attribute__((packed));

    /**
     * @brief Player information for client list
     */
    struct PlayerInfo {
        uint32_t player_id;      ///< Player ID
        uint8_t ready_state;     ///< 1 = ready, 0 = not ready
        char name[32];           ///< Player name (null-terminated)
    } __attribute__((packed));

    /**
     * @brief Client list update message
     */
    struct ClientListUpdate {
        uint8_t player_count;    ///< Number of players in the list
        PlayerInfo players[8];   ///< Array of player info (max 8 players)
    } __attribute__((packed));

    /**
     * @brief Start game signal message
     */
    struct StartGame {
        uint32_t timestamp;      ///< Server timestamp when game starts
    } __attribute__((packed));

    /**
     * @brief Position update message
     */
    struct PositionUpdate {
        uint32_t entity_id;      ///< Entity ID
        float x, y;              ///< Position coordinates
        float vx, vy;            ///< Velocity (optional)
        uint32_t timestamp;      ///< Client timestamp
    } __attribute__((packed));

    /**
     * @brief Entity creation message
     */
    struct EntityCreate {
        uint32_t entity_id;      ///< New entity ID
        uint8_t entity_type;     ///< Type of entity
        float x, y;              ///< Initial position
        float health;            ///< Initial health
    } __attribute__((packed));

    /**
     * @brief Entity destruction message
     */
    struct EntityDestroy {
        uint32_t entity_id;      ///< Entity to destroy
        uint8_t reason;          ///< Destruction reason
    } __attribute__((packed));

    /**
     * @brief Player shooting message
     */
    struct PlayerShoot {
        uint32_t player_id;      ///< Shooting player
        float start_x, start_y;  ///< Bullet start position
        float dir_x, dir_y;      ///< Bullet direction
        uint8_t weapon_type;     ///< Type of weapon used
    } __attribute__((packed));

    /**
     * @brief Ping message
     */
    struct Ping {
        uint32_t timestamp;      ///< Client timestamp
        uint32_t sequence;       ///< Ping sequence number
    } __attribute__((packed));

    /**
     * @brief Pong response
     */
    struct Pong {
        uint32_t timestamp;      ///< Original client timestamp
        uint32_t sequence;       ///< Original sequence number
        uint32_t server_time;    ///< Server timestamp
    } __attribute__((packed));

    // ============================================================================
    // Serialization Helpers
    // ============================================================================

    /**
     * @brief Packet builder for creating well-formed packets
     */
    class PacketBuilder {
    public:
        PacketBuilder();

        /**
         * @brief Start building a new packet
         * @param message_type Type of message
         * @param flags Packet flags
         */
        void begin_packet(uint8_t message_type, PacketFlags flags = PacketFlags::NONE);

        /**
         * @brief Add data to the packet
         * @param data Data to add
         * @param size Size of data
         */
        void add_data(const void* data, size_t size);

        /**
         * @brief Add a typed structure to the packet
         * @param structure Structure to add
         */
        template<typename T>
        void add_struct(const T& structure) {
            add_data(&structure, sizeof(T));
        }

        /**
         * @brief Finalize the packet and get the buffer
         * @return Packet buffer
         */
        std::vector<uint8_t> finalize();

        /**
         * @brief Get current packet size
         */
        size_t size() const { return buffer_.size(); }

        /**
         * @brief Clear the builder for reuse
         */
        void clear();

    private:
        std::vector<uint8_t> buffer_;
        size_t header_offset_;
    };

    /**
     * @brief Packet parser for reading incoming packets
     */
    class PacketParser {
    public:
        /**
         * @brief Parse a packet from buffer
         * @param data Packet data
         * @param size Size of packet data
         */
        PacketParser(const char* data, size_t size);

        /**
         * @brief Check if packet is valid
         */
        bool is_valid() const { return valid_; }

        /**
         * @brief Get packet header
         */
        const PacketHeader& get_header() const { return header_; }

        /**
         * @brief Get payload data
         */
        const char* get_payload() const { return payload_; }

        /**
         * @brief Get payload size
         */
        size_t get_payload_size() const { return header_.payload_size; }

        /**
         * @brief Parse a typed structure from payload
         * @param offset Offset in payload
         * @return Pointer to structure or nullptr if invalid
         */
        template<typename T>
        const T* parse_struct(size_t offset = 0) const {
            if (!valid_ || offset + sizeof(T) > header_.payload_size) {
                return nullptr;
            }
            return reinterpret_cast<const T*>(payload_ + offset);
        }

    private:
        PacketHeader header_;
        const char* payload_;
        bool valid_;
    };

    // ============================================================================
    // Utility Functions
    // ============================================================================

    /**
     * @brief Create a simple packet with just a message type
     */
    std::vector<uint8_t> create_simple_packet(uint8_t message_type, PacketFlags flags = PacketFlags::NONE);

    /**
     * @brief Create a packet with a typed payload
     */
    template<typename T>
    std::vector<uint8_t> create_packet(uint8_t message_type, const T& payload, PacketFlags flags = PacketFlags::NONE) {
        PacketBuilder builder;
        builder.begin_packet(message_type, flags);
        builder.add_struct(payload);
        return builder.finalize();
    }

    /**
     * @brief Validate packet integrity
     */
    bool validate_packet(const char* data, size_t size);

} // namespace RType::Protocol