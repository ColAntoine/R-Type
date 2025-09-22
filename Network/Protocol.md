# Keller Protocol Networking Standard (KPN) v1.0

**RFC-Style Protocol Specification**

---

## Table of Contents

1. [Protocol Overview](#protocol-overview)
2. [Core Architecture](#core-architecture)
3. [Packet Structure](#packet-structure)
4. [Common Protocol Layer](#common-protocol-layer)
5. [R-Type Game Protocol](#r-type-game-protocol)
6. [Implementation Examples](#implementation-examples)
7. [Advanced Features](#advanced-features)
8. [Security Considerations](#security-considerations)
9. [Future Extensibility](#future-extensibility)

---

## Protocol Overview

The Keller Protocol Networking Standard (KPN) is a binary, UDP-based protocol designed for real-time multiplayer gaming. It supports multiple game types through a modular architecture while providing robust networking features for competitive gameplay.

### Key Features

- **Binary Protocol**: Optimized for performance and bandwidth efficiency
- **UDP Transport**: Low-latency communication with optional reliability layer
- **Multi-Game Support**: Common base protocol with game-specific extensions
- **Version Management**: Forward and backward compatibility
- **Security**: Built-in protection against malformed packets and attacks
- **Extensibility**: Easy addition of new games and features without protocol changes

### Design Principles

- **Modularity**: Clear separation between common and game-specific functionality
- **Performance**: Minimal overhead and maximum throughput
- **Reliability**: Optional guaranteed delivery for critical messages
- **Scalability**: Support for multiple game instances and lobby systems
- **Robustness**: Graceful handling of network issues and malformed data

---

## Core Architecture

### Protocol Layers

```
┌─────────────────────────────────────┐
│        Application Layer            │
│    (Game Logic & User Interface)    │
├─────────────────────────────────────┤
│        Game-Specific Layer          │
│     (R-Type, Pong, etc.)            │
├─────────────────────────────────────┤
│        Common Protocol Layer        │
│   (Connection, Lobby, Reliability)  │
├─────────────────────────────────────┤
│         Transport Layer             │
│           (UDP/TCP)                 │
└─────────────────────────────────────┘
```

### Message Categories

1. **System Messages** (0x00-0x3F): Connection, authentication, error handling
2. **Lobby Messages** (0x40-0x7F): Room management, matchmaking, user lists
3. **Game State Messages** (0x80-0xBF): Generic game state management
4. **Game-Specific Messages** (0xC0-0xFF): Per-game protocol extensions

---

## Packet Structure

### Base Packet Header (8 bytes)

```cpp
struct PacketHeader {
    uint16_t magic;           // Protocol magic number: 0x504E ("PN")
    uint8_t  version;         // Protocol version (current: 1)
    uint8_t  flags;           // Packet flags (see below)
    uint8_t  message_type;    // Message type identifier
    uint8_t  game_id;         // Game type identifier
    uint16_t payload_size;    // Payload size in bytes
} __attribute__((packed));
```

### Packet Flags (1 byte)

```
Bit 7: Reliable (requires ACK)
Bit 6: Compressed (payload is compressed)
Bit 5: Fragmented (part of larger message)
Bit 4: Priority (high priority message)
Bit 3-0: Reserved for future use
```

### Extended Headers (Optional)

#### Reliability Header (4 bytes) - when Reliable flag is set

```cpp
struct ReliabilityHeader {
    uint16_t sequence_id;     // Sequence number for ordering
    uint16_t ack_id;          // Acknowledgment number
} __attribute__((packed));
```

#### Compression Header (2 bytes) - when Compressed flag is set

```cpp
struct CompressionHeader {
    uint16_t original_size;   // Original payload size before compression
} __attribute__((packed));
```

#### Fragmentation Header (4 bytes) - when Fragmented flag is set

```cpp
struct FragmentHeader {
    uint16_t fragment_id;     // Fragment identifier
    uint8_t  fragment_index;  // Current fragment index (0-based)
    uint8_t  total_fragments; // Total number of fragments
} __attribute__((packed));
```

---

## Common Protocol Layer

### Game Identifiers

```cpp
enum class GameType : uint8_t {
    SYSTEM     = 0x00,  // System/Lobby messages
    RTYPE      = 0x01,  // R-Type game
    LOL        = 0x02,  // League game
    R6         = 0x03,  // R6 game
    // ... extensible up to 255 games
};
```

### System Messages (0x00-0x3F)

#### Connection Messages

```cpp
// CLIENT_CONNECT (0x01)
struct ClientConnect {
    uint8_t protocol_version;
    uint32_t client_version;
    char username[32];
    uint8_t capabilities;  // Client features bit field
} __attribute__((packed));

// SERVER_ACCEPT (0x02)
struct ServerAccept {
    uint32_t client_id;
    uint32_t server_version;
    uint8_t server_capabilities;
} __attribute__((packed));

// CONNECTION_REJECTED (0x03)
struct ConnectionRejected {
    uint8_t reason_code;
    char reason_message[64];
} __attribute__((packed));

// HEARTBEAT (0x04)
struct Heartbeat {
    uint32_t timestamp;
    uint16_t client_id;
} __attribute__((packed));

// DISCONNECT (0x05)
struct Disconnect {
    uint8_t reason_code;
    char reason_message[64];
} __attribute__((packed));
```

#### Error Handling

```cpp
// ERROR_MESSAGE (0x10)
struct ErrorMessage {
    uint8_t error_code;
    uint8_t severity;  // 0=Info, 1=Warning, 2=Error, 3=Critical
    char error_text[128];
} __attribute__((packed));

// MALFORMED_PACKET (0x11)
struct MalformedPacket {
    uint32_t original_size;
    uint8_t error_position;
    uint8_t error_type;
} __attribute__((packed));
```

### Lobby Messages (0x40-0x7F)

#### Room Management

```cpp
// CREATE_ROOM (0x40)
struct CreateRoom {
    GameType game_type;
    uint8_t max_players;
    uint8_t room_settings_size;
    // Followed by game-specific room settings
} __attribute__((packed));

// JOIN_ROOM (0x41)
struct JoinRoom {
    uint32_t room_id;
    char password[32];  // Optional room password
} __attribute__((packed));

// ROOM_LIST_REQUEST (0x42)
struct RoomListRequest {
    GameType game_filter;  // 0x00 for all games
    uint8_t filters;       // Bit field for various filters
} __attribute__((packed));

// ROOM_LIST_RESPONSE (0x43)
struct RoomInfo {
    uint32_t room_id;
    GameType game_type;
    uint8_t current_players;
    uint8_t max_players;
    uint8_t status;  // 0=Waiting, 1=In-Game, 2=Full
    char room_name[32];
} __attribute__((packed));

// PLAYER_JOINED (0x44)
struct PlayerJoined {
    uint32_t player_id;
    char username[32];
    uint8_t player_index;
} __attribute__((packed));

// PLAYER_LEFT (0x45)
struct PlayerLeft {
    uint32_t player_id;
    uint8_t reason;  // 0=Voluntary, 1=Disconnected, 2=Kicked
} __attribute__((packed));
```

### Game State Messages (0x80-0xBF)

#### Generic Game Control

```cpp
// GAME_START (0x80)
struct GameStart {
    uint32_t game_session_id;
    uint32_t random_seed;
    uint8_t game_settings_size;
    // Followed by game-specific settings
} __attribute__((packed));

// GAME_END (0x81)
struct GameEnd {
    uint8_t end_reason;  // 0=Normal, 1=Disconnection, 2=Error
    uint8_t winner_count;
    uint32_t winners[4]; // Player IDs of winners
} __attribute__((packed));

// GAME_PAUSE (0x82)
struct GamePause {
    uint32_t player_id;  // Who requested pause
    uint8_t pause_reason;
} __attribute__((packed));

// PLAYER_READY (0x83)
struct PlayerReady {
    uint32_t player_id;
    uint8_t ready_state;  // 0=Not ready, 1=Ready
} __attribute__((packed));
```

---

## R-Type Game Protocol

### R-Type Message Types (0xC0-0xFF)

#### Entity Management

```cpp
// ENTITY_SPAWN (0xC0)
struct EntitySpawn {
    uint32_t entity_id;
    uint8_t entity_type;    // See EntityType enum
    float position_x;
    float position_y;
    float velocity_x;
    float velocity_y;
    uint8_t entity_data_size;
    // Followed by entity-specific data
} __attribute__((packed));

// ENTITY_UPDATE (0xC1)
struct EntityUpdate {
    uint32_t entity_id;
    uint32_t timestamp;     // Server timestamp
    float position_x;
    float position_y;
    float velocity_x;
    float velocity_y;
    uint8_t state_flags;    // Health, shields, etc.
} __attribute__((packed));

// ENTITY_DESTROY (0xC2)
struct EntityDestroy {
    uint32_t entity_id;
    uint8_t destruction_type;  // 0=Normal, 1=Explosion, 2=Fade
    float destruction_x;       // Destruction effect position
    float destruction_y;
} __attribute__((packed));
```

#### Player Actions

```cpp
// PLAYER_INPUT (0xC8)
struct PlayerInput {
    uint32_t player_id;
    uint32_t timestamp;
    uint8_t input_flags;    // Movement and action flags
    int16_t movement_x;     // -1000 to 1000 (normalized)
    int16_t movement_y;     // -1000 to 1000 (normalized)
} __attribute__((packed));

// Input flags bit field
enum class InputFlags : uint8_t {
    MOVE_UP    = 0x01,
    MOVE_DOWN  = 0x02,
    MOVE_LEFT  = 0x04,
    MOVE_RIGHT = 0x08,
    SHOOT      = 0x10,
    SPECIAL    = 0x20,
    FORCE_CMD  = 0x40,
    RESERVED   = 0x80
};

// PLAYER_SHOOT (0xC9)
struct PlayerShoot {
    uint32_t player_id;
    uint32_t timestamp;
    float start_x;
    float start_y;
    float direction_x;
    float direction_y;
    uint8_t weapon_type;
    uint8_t power_level;
} __attribute__((packed));
```

#### R-Type Specific Entities

```cpp
enum class RTypeEntityType : uint8_t {
    PLAYER_SHIP     = 0x01,
    ENEMY_BASIC     = 0x10,
    ENEMY_SNAKE     = 0x11,
    ENEMY_TURRET    = 0x12,
    BOSS_DOBKERATOPS = 0x20,
    MISSILE_PLAYER  = 0x30,
    MISSILE_ENEMY   = 0x31,
    FORCE_ORB       = 0x40,
    POWERUP_SPEED   = 0x50,
    POWERUP_MISSILE = 0x51,
    POWERUP_FORCE   = 0x52,
    OBSTACLE        = 0x60,
    BACKGROUND_STAR = 0x70
};

// FORCE_COMMAND (0xCA) - R-Type Force orb control
struct ForceCommand {
    uint32_t player_id;
    uint32_t force_id;
    uint8_t command_type;   // 0=Attach Front, 1=Attach Back, 2=Detach, 3=Recall
    float target_x;         // For detach position
    float target_y;
} __attribute__((packed));

// WAVE_SPAWN (0xCB) - Enemy wave management
struct WaveSpawn {
    uint16_t wave_id;
    uint8_t enemy_count;
    uint8_t formation_type;
    float spawn_x;
    float spawn_y;
    uint8_t enemy_types[16]; // Array of enemy types
} __attribute__((packed));
```

#### Game Events

```cpp
// COLLISION_EVENT (0xD0)
struct CollisionEvent {
    uint32_t entity1_id;
    uint32_t entity2_id;
    float collision_x;
    float collision_y;
    uint8_t collision_type; // 0=Damage, 1=Pickup, 2=Destruction
    uint16_t damage_dealt;
} __attribute__((packed));

// SCORE_UPDATE (0xD1)
struct ScoreUpdate {
    uint32_t player_id;
    uint32_t score_delta;
    uint32_t total_score;
    uint8_t score_reason;   // 0=Enemy kill, 1=Bonus, 2=Powerup
} __attribute__((packed));

// LEVEL_EVENT (0xD2)
struct LevelEvent {
    uint8_t event_type;     // 0=Level start, 1=Boss appear, 2=Level complete
    uint16_t level_number;
    uint32_t event_data;    // Additional event-specific data
} __attribute__((packed));
```

---

## Implementation Examples

### C++ Client Implementation

```cpp
#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include <functional>

namespace KPN {

class NetworkClient {
private:
    struct PacketBuffer {
        std::vector<uint8_t> data;
        size_t size;

        PacketBuffer(size_t capacity = 1024) : data(capacity), size(0) {}
    };

    std::unique_ptr<PacketBuffer> send_buffer_;
    std::unique_ptr<PacketBuffer> recv_buffer_;

    // Reliability tracking
    uint16_t next_sequence_id_;
    std::map<uint16_t, std::chrono::steady_clock::time_point> pending_acks_;

    // Callbacks
    std::function<void(const PacketHeader&, const uint8_t*)> message_handler_;

public:
    NetworkClient()
        : send_buffer_(std::make_unique<PacketBuffer>())
        , recv_buffer_(std::make_unique<PacketBuffer>())
        , next_sequence_id_(1) {}

    // Core packet operations
    template<typename T>
    bool SendMessage(uint8_t message_type, GameType game_id, const T& payload, bool reliable = false);

    bool SendRawMessage(uint8_t message_type, GameType game_id, const uint8_t* data, size_t size, bool reliable = false);

    void ProcessIncomingPackets();

    // Message serialization helpers
    template<typename T>
    void SerializeMessage(PacketBuffer& buffer, const T& message);

    template<typename T>
    bool DeserializeMessage(const uint8_t* data, size_t size, T& message);

    // Connection management
    bool Connect(const std::string& server_address, uint16_t port, const std::string& username);
    void Disconnect();
    bool IsConnected() const;

    // R-Type specific helpers
    void SendPlayerInput(uint32_t player_id, InputFlags flags, int16_t move_x, int16_t move_y);
    void SendShootCommand(uint32_t player_id, float start_x, float start_y, float dir_x, float dir_y, uint8_t weapon_type);
    void SendForceCommand(uint32_t player_id, uint32_t force_id, uint8_t command, float target_x, float target_y);

    // Event handlers
    void SetMessageHandler(std::function<void(const PacketHeader&, const uint8_t*)> handler);

private:
    // Internal packet processing
    bool ValidatePacket(const PacketHeader& header, size_t total_size);
    void HandleReliablePacket(const PacketHeader& header, const uint8_t* payload);
    void SendAcknowledgment(uint16_t sequence_id);
    void ProcessReliabilityTimeouts();

    // Compression and decompression
    std::vector<uint8_t> CompressPayload(const uint8_t* data, size_t size);
    std::vector<uint8_t> DecompressPayload(const uint8_t* data, size_t compressed_size, size_t original_size);
};

// Template implementations
template<typename T>
bool NetworkClient::SendMessage(uint8_t message_type, GameType game_id, const T& payload, bool reliable) {
    send_buffer_->size = 0;

    // Serialize the payload first
    SerializeMessage(*send_buffer_, payload);

    return SendRawMessage(message_type, game_id, send_buffer_->data.data(), send_buffer_->size, reliable);
}

template<typename T>
void NetworkClient::SerializeMessage(PacketBuffer& buffer, const T& message) {
    static_assert(std::is_trivially_copyable_v<T>, "Message must be trivially copyable");

    if (buffer.data.size() < sizeof(T)) {
        buffer.data.resize(sizeof(T));
    }

    std::memcpy(buffer.data.data(), &message, sizeof(T));
    buffer.size = sizeof(T);
}

template<typename T>
bool NetworkClient::DeserializeMessage(const uint8_t* data, size_t size, T& message) {
    static_assert(std::is_trivially_copyable_v<T>, "Message must be trivially copyable");

    if (size < sizeof(T)) {
        return false; // Insufficient data
    }

    std::memcpy(&message, data, sizeof(T));
    return true;
}

} // namespace KPN
```

### C++ Server Implementation

```cpp
#pragma once
#include "NetworkClient.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>

namespace KPN {

struct ClientSession {
    uint32_t client_id;
    std::string username;
    std::chrono::steady_clock::time_point last_heartbeat;
    GameType current_game;
    uint32_t room_id;
    bool is_authenticated;

    // Reliability tracking per client
    uint16_t last_received_sequence;
    std::map<uint16_t, bool> received_packets;
};

class NetworkServer {
private:
    std::atomic<bool> running_;
    std::thread network_thread_;
    std::thread game_thread_;

    std::mutex clients_mutex_;
    std::unordered_map<uint32_t, std::unique_ptr<ClientSession>> clients_;
    uint32_t next_client_id_;

    // Game instance management
    std::mutex games_mutex_;
    std::unordered_map<uint32_t, GameInstance> active_games_;
    uint32_t next_game_id_;

    // Message processing
    std::queue<std::pair<uint32_t, std::vector<uint8_t>>> incoming_messages_;
    std::mutex message_queue_mutex_;

public:
    NetworkServer(uint16_t port) : running_(false), next_client_id_(1), next_game_id_(1) {}

    // Server lifecycle
    bool Start();
    void Stop();
    bool IsRunning() const { return running_; }

    // Client management
    uint32_t RegisterClient(const std::string& username);
    void DisconnectClient(uint32_t client_id, uint8_t reason);
    bool IsClientConnected(uint32_t client_id) const;

    // Game instance management
    uint32_t CreateGameInstance(GameType game_type, uint8_t max_players);
    bool JoinGame(uint32_t client_id, uint32_t game_id);
    void LeaveGame(uint32_t client_id);

    // Broadcasting
    void BroadcastToGame(uint32_t game_id, uint8_t message_type, const uint8_t* data, size_t size);
    void BroadcastToAll(uint8_t message_type, const uint8_t* data, size_t size);
    void SendToClient(uint32_t client_id, uint8_t message_type, GameType game_id, const uint8_t* data, size_t size, bool reliable = false);

    // R-Type specific game logic
    void ProcessPlayerInput(uint32_t client_id, const PlayerInput& input);
    void ProcessPlayerShoot(uint32_t client_id, const PlayerShoot& shoot);
    void ProcessForceCommand(uint32_t client_id, const ForceCommand& command);

    // Entity management for R-Type
    uint32_t SpawnEntity(uint32_t game_id, RTypeEntityType type, float x, float y, float vel_x, float vel_y);
    void UpdateEntity(uint32_t game_id, uint32_t entity_id, float x, float y, float vel_x, float vel_y);
    void DestroyEntity(uint32_t game_id, uint32_t entity_id, uint8_t destruction_type);

private:
    // Network threads
    void NetworkThreadProc();
    void GameThreadProc();

    // Message processing
    void ProcessIncomingMessage(uint32_t client_id, const uint8_t* data, size_t size);
    void HandleSystemMessage(uint32_t client_id, uint8_t message_type, const uint8_t* payload, size_t size);
    void HandleLobbyMessage(uint32_t client_id, uint8_t message_type, const uint8_t* payload, size_t size);
    void HandleGameMessage(uint32_t client_id, uint8_t message_type, const uint8_t* payload, size_t size);

    // Reliability and error handling
    void ValidateAndProcessPacket(uint32_t client_id, const uint8_t* data, size_t size);
    void SendErrorMessage(uint32_t client_id, uint8_t error_code, const std::string& message);
    void HandleMalformedPacket(uint32_t client_id, const uint8_t* data, size_t size);

    // Heartbeat and connection management
    void ProcessHeartbeats();
    void CheckClientTimeouts();
};

// Game-specific server components
class RTypeGameServer {
private:
    NetworkServer* network_server_;
    uint32_t game_id_;

    // Game state
    std::unordered_map<uint32_t, RTypeEntity> entities_;
    std::unordered_map<uint32_t, RTypePlayer> players_;
    uint32_t next_entity_id_;

    // Game timing
    std::chrono::steady_clock::time_point last_update_;
    const std::chrono::milliseconds update_interval_{16}; // ~60 FPS

    // Wave management
    std::vector<EnemyWave> enemy_waves_;
    size_t current_wave_index_;

public:
    RTypeGameServer(NetworkServer* server, uint32_t game_id)
        : network_server_(server), game_id_(game_id), next_entity_id_(1), current_wave_index_(0) {}

    // Game lifecycle
    void StartGame();
    void UpdateGame();
    void EndGame();

    // Player management
    void AddPlayer(uint32_t client_id);
    void RemovePlayer(uint32_t client_id);
    void ProcessPlayerAction(uint32_t client_id, const PlayerInput& input);

    // Entity management
    void SpawnEnemyWave(const EnemyWave& wave);
    void UpdateEntities(float delta_time);
    void CheckCollisions();
    void BroadcastEntityUpdates();

    // Game events
    void HandleCollision(uint32_t entity1, uint32_t entity2);
    void HandlePlayerDeath(uint32_t player_id);
    void HandleEnemyDestroyed(uint32_t enemy_id, uint32_t killer_id);
};

} // namespace KPN
```

### Usage Examples

#### Client Connection Example

```cpp
#include "NetworkClient.h"

int main() {
    KPN::NetworkClient client;

    // Lag mitigation components
    ClientSidePrediction prediction;
    EntityInterpolator interpolator;
    InputPredictor input_predictor;

    // Set up message handler with lag mitigation
    client.SetMessageHandler([&](const KPN::PacketHeader& header, const uint8_t* payload) {
        switch (header.message_type) {
            case 0xC1: { // ENTITY_UPDATE
                KPN::EntityUpdate update;
                if (client.DeserializeMessage(payload, header.payload_size, update)) {
                    if (update.entity_id == GetPlayerId()) {
                        // Reconcile local prediction with server state
                        prediction.ReconcileWithServer(update);
                    } else {
                        // Interpolate remote entity movement
                        float current_x = GetEntityX(update.entity_id);
                        float current_y = GetEntityY(update.entity_id);
                        interpolator.StartInterpolation(update.entity_id,
                                                     current_x, current_y,
                                                     update.position_x, update.position_y,
                                                     100); // 100ms interpolation
                    }
                }
                break;
            }
            case 0xC8: { // PLAYER_INPUT (echoed back from server)
                KPN::PlayerInput echoed_input;
                if (client.DeserializeMessage(payload, header.payload_size, echoed_input)) {
                    input_predictor.OnServerAcknowledgment(echoed_input.timestamp);
                }
                break;
            }
            case 0xD0: { // COLLISION_EVENT
                KPN::CollisionEvent collision;
                if (client.DeserializeMessage(payload, header.payload_size, collision)) {
                    // Handle collision effect
                    PlayCollisionEffect(collision.collision_x, collision.collision_y);
                }
                break;
            }
        }
    });

    // Connect to server
    if (client.Connect("localhost", 4242, "Player1")) {
        std::cout << "Connected to server!" << std::endl;

        // Game loop with lag mitigation
        while (client.IsConnected()) {
            // Process input with prediction
            InputFlags flags = 0;
            int16_t move_x = 0, move_y = 0;

            if (IsKeyDown(KEY_UP))    { flags |= InputFlags::MOVE_UP; move_y = -1000; }
            if (IsKeyDown(KEY_DOWN))  { flags |= InputFlags::MOVE_DOWN; move_y = 1000; }
            if (IsKeyDown(KEY_LEFT))  { flags |= InputFlags::MOVE_LEFT; move_x = -1000; }
            if (IsKeyDown(KEY_RIGHT)) { flags |= InputFlags::MOVE_RIGHT; move_x = 1000; }

            // Apply input prediction immediately for responsive controls
            if (flags != 0 || move_x != 0 || move_y != 0) {
                input_predictor.SendPredictedInput(flags, move_x, move_y);
            }

            // Handle shooting (can also be predicted)
            if (IsKeyPressed(KEY_SPACE)) {
                client.SendShootCommand(GetPlayerId(), GetPlayerX(), GetPlayerY(), 1.0f, 0.0f, 1);
            }

            // Update interpolations for smooth remote entity movement
            interpolator.UpdateInterpolations();

            // Process incoming messages
            client.ProcessIncomingPackets();

            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        }
    }

    return 0;
}
```

---

## Advanced Features

### Reliability Layer

The protocol implements an optional reliability layer over UDP to ensure critical messages are delivered:

#### Acknowledgment System

```cpp
// ACK Message (0x06)
struct AckMessage {
    uint16_t ack_sequence_id;
    uint32_t timestamp;
} __attribute__((packed));

// Reliability configuration
struct ReliabilityConfig {
    uint32_t timeout_ms = 200;        // Retransmission timeout
    uint8_t max_retries = 3;          // Maximum retry attempts
    bool enable_ordering = true;      // Enforce packet ordering
    bool enable_deduplication = true; // Remove duplicate packets
};
```

#### Implementation Details

- **Sequence Numbers**: Each reliable packet gets a unique sequence number
- **Acknowledgments**: Receiver sends ACK for each reliable packet
- **Retransmission**: Sender retransmits if no ACK received within timeout
- **Ordering**: Optional in-order delivery guarantee
- **Deduplication**: Automatic removal of duplicate packets

### Compression Support

Optional payload compression for bandwidth optimization:

#### Compression Algorithms

```cpp
enum class CompressionType : uint8_t {
    NONE        = 0x00,
    LZ4         = 0x01,  // Fast compression for real-time
    ZLIB        = 0x02,  // Better compression ratio
    GAME_DELTA  = 0x03   // Game-specific delta compression
};

// Compression threshold configuration
struct CompressionConfig {
    size_t min_payload_size = 64;     // Only compress if payload > 64 bytes
    CompressionType default_type = CompressionType::LZ4;
    uint8_t compression_level = 1;    // 1-9 for zlib, ignored for LZ4
};
```

#### Delta Compression for Game State

```cpp
// For entity updates, only send changed fields
struct DeltaEntityUpdate {
    uint32_t entity_id;
    uint8_t changed_fields;  // Bit field indicating which fields changed
    // Optional fields based on changed_fields bits:
    // Bit 0: position_x (float)
    // Bit 1: position_y (float)
    // Bit 2: velocity_x (float)
    // Bit 3: velocity_y (float)
    // Bit 4: health (uint16_t)
    // Bit 5: state_flags (uint8_t)
} __attribute__((packed));
```

### Fragmentation Support

Large messages can be automatically fragmented:

#### Fragmentation Process

1. **Automatic Splitting**: Messages > MTU are split into fragments
2. **Fragment Tracking**: Each fragment has ID, index, and total count
3. **Reassembly**: Receiver reconstructs original message from fragments
4. **Timeout Handling**: Incomplete fragments are discarded after timeout

```cpp
class FragmentationManager {
private:
    struct FragmentGroup {
        std::vector<std::vector<uint8_t>> fragments;
        std::chrono::steady_clock::time_point first_fragment_time;
        uint8_t received_count;
        uint8_t total_fragments;
    };

    std::unordered_map<uint16_t, FragmentGroup> pending_fragments_;

public:
    std::vector<uint8_t> ProcessFragment(const FragmentHeader& header, const uint8_t* data, size_t size);
    void CleanupExpiredFragments();
};
```

### Multi-Instance Server Support

Support for multiple concurrent game instances:

#### Instance Management

```cpp
struct GameInstance {
    uint32_t instance_id;
    GameType game_type;
    std::vector<uint32_t> player_ids;
    uint8_t max_players;
    GameState state;
    std::unique_ptr<GameLogic> game_logic;

    // Instance-specific settings
    std::unordered_map<std::string, std::string> settings;
};

class MultiInstanceServer {
private:
    std::unordered_map<uint32_t, std::unique_ptr<GameInstance>> instances_;
    std::mutex instances_mutex_;

public:
    uint32_t CreateInstance(GameType type, const std::unordered_map<std::string, std::string>& settings);
    bool JoinInstance(uint32_t instance_id, uint32_t client_id);
    void BroadcastToInstance(uint32_t instance_id, const uint8_t* message, size_t size);
    void UpdateAllInstances();
};
```

### Performance Optimizations

#### Batch Message Processing

```cpp
// Multiple messages can be batched in a single UDP packet
struct BatchedMessage {
    uint8_t message_count;
    // Followed by:
    // For each message:
    //   uint8_t message_type
    //   uint16_t message_size
    //   uint8_t message_data[message_size]
} __attribute__((packed));
```

#### Message Priority System

```cpp
enum class MessagePriority : uint8_t {
    LOW       = 0,  // Background updates, non-critical
    NORMAL    = 1,  // Regular game updates
    HIGH      = 2,  // Player actions, important events
    CRITICAL  = 3   // Connection/error messages
};
```

### Lag Mitigation and Real-time Gameplay

For fast-paced multiplayer games like R-Type, network lag can severely impact gameplay quality. The protocol provides foundational support through timestamps and sequencing, but effective lag mitigation requires game-level implementation of the following techniques:

#### Client-Side Prediction

Clients predict their own movement immediately upon input, providing instant responsiveness:

```cpp
class ClientSidePrediction {
private:
    struct PredictedState {
        uint32_t timestamp;
        float position_x, position_y;
        float velocity_x, velocity_y;
        InputFlags last_input;
    };

    std::deque<PredictedState> prediction_history_;
    uint32_t last_server_update_timestamp_;

public:
    void ApplyInput(InputFlags input, int16_t move_x, int16_t move_y) {
        // Immediately update local player position
        UpdateLocalPlayer(move_x, move_y);

        // Store prediction for later reconciliation
        prediction_history_.push_back({
            GetCurrentTimestamp(),
            GetPlayerX(), GetPlayerY(),
            GetPlayerVelX(), GetPlayerVelY(),
            input
        });

        // Send input to server with timestamp
        client.SendPlayerInput(player_id_, input, move_x, move_y);
    }

    void ReconcileWithServer(const EntityUpdate& server_update) {
        // Find corresponding prediction
        auto it = std::find_if(prediction_history_.begin(), prediction_history_.end(),
            [&](const PredictedState& pred) {
                return pred.timestamp >= server_update.timestamp;
            });

        if (it != prediction_history_.end()) {
            // Calculate position error
            float error_x = server_update.position_x - it->position_x;
            float error_y = server_update.position_y - it->position_y;

            // Smoothly correct position (lerp over several frames)
            if (std::abs(error_x) > 0.1f || std::abs(error_y) > 0.1f) {
                StartPositionCorrection(error_x, error_y);
            }

            // Remove reconciled predictions
            prediction_history_.erase(prediction_history_.begin(), it);
        }
    }
};
```

#### Entity Interpolation

Smooth movement between server updates prevents jerky motion:

```cpp
class EntityInterpolator {
private:
    struct InterpolationState {
        uint32_t start_time;
        float start_x, start_y;
        float end_x, end_y;
        float duration_ms;
    };

    std::unordered_map<uint32_t, InterpolationState> interpolating_entities_;

public:
    void StartInterpolation(uint32_t entity_id, float current_x, float current_y,
                           float target_x, float target_y, uint32_t update_interval_ms) {
        interpolating_entities_[entity_id] = {
            GetCurrentTime(),
            current_x, current_y,
            target_x, target_y,
            static_cast<float>(update_interval_ms)
        };
    }

    void UpdateInterpolations() {
        uint32_t current_time = GetCurrentTime();

        for (auto it = interpolating_entities_.begin(); it != interpolating_entities_.end(); ) {
            const auto& state = it->second;
            float elapsed = current_time - state.start_time;
            float t = std::min(elapsed / state.duration_ms, 1.0f);

            // Smooth interpolation (ease-in-out)
            t = t * t * (3.0f - 2.0f * t);

            float interpolated_x = state.start_x + (state.end_x - state.start_x) * t;
            float interpolated_y = state.start_y + (state.end_y - state.start_y) * t;

            UpdateEntityPosition(it->first, interpolated_x, interpolated_y);

            if (t >= 1.0f) {
                it = interpolating_entities_.erase(it);
            } else {
                ++it;
            }
        }
    }
};
```

#### Input Prediction and Reconciliation

Send inputs ahead of time and reconcile when server confirms:

```cpp
class InputPredictor {
private:
    struct PendingInput {
        uint32_t sequence_id;
        uint32_t timestamp;
        InputFlags input;
        int16_t move_x, move_y;
        bool acknowledged;
    };

    std::deque<PendingInput> input_buffer_;
    uint32_t next_input_sequence_;

public:
    void SendPredictedInput(InputFlags input, int16_t move_x, int16_t move_y) {
        PendingInput pending = {
            next_input_sequence_++,
            GetCurrentTimestamp(),
            input, move_x, move_y,
            false
        };

        input_buffer_.push_back(pending);

        // Send to server
        client.SendPlayerInput(player_id_, input, move_x, move_y);

        // Apply locally immediately
        ApplyInputLocally(input, move_x, move_y);
    }

    void OnServerAcknowledgment(uint32_t acknowledged_sequence) {
        // Mark inputs as acknowledged up to this sequence
        for (auto& input : input_buffer_) {
            if (input.sequence_id <= acknowledged_sequence) {
                input.acknowledged = true;
            }
        }

        // Remove old acknowledged inputs
        while (!input_buffer_.empty() && input_buffer_.front().acknowledged) {
            input_buffer_.pop_front();
        }
    }

    void OnServerCorrection(const EntityUpdate& server_state) {
        // If server state differs significantly from prediction, rewind and replay
        if (NeedsRewind(server_state)) {
            RewindAndReplay(server_state);
        }
    }
};
```

#### Lag Compensation Techniques

##### Server-Side Lag Compensation

```cpp
class LagCompensation {
private:
    struct ClientHistory {
        uint32_t client_id;
        std::deque<EntityState> position_history_;
        std::chrono::milliseconds history_length_{1000}; // Keep 1 second of history
    };

    std::unordered_map<uint32_t, ClientHistory> client_histories_;

public:
    void StoreClientPosition(uint32_t client_id, const EntityState& state) {
        auto& history = client_histories_[client_id];
        history.position_history_.push_back(state);

        // Remove old entries
        auto cutoff_time = std::chrono::steady_clock::now() - history.history_length_;
        while (!history.position_history_.empty() &&
               history.position_history_.front().timestamp < cutoff_time) {
            history.position_history_.pop_front();
        }
    }

    const EntityState* GetClientPositionAtTime(uint32_t client_id, uint32_t timestamp) const {
        const auto& history = client_histories_.at(client_id);

        // Find the state closest to the requested timestamp
        for (auto it = history.position_history_.rbegin(); it != history.position_history_.rend(); ++it) {
            if (it->timestamp <= timestamp) {
                return &(*it);
            }
        }

        return nullptr; // No suitable history found
    }
};
```

##### Implementation Guidelines

1. **Always use client-side prediction** for the local player to ensure responsive controls
2. **Implement entity interpolation** with 100-200ms buffers for smooth remote entity movement
3. **Send inputs at fixed intervals** (e.g., 60Hz) regardless of server update rate
4. **Use server reconciliation** to correct prediction errors without jarring the player
5. **Consider lag compensation** on the server for fair hit detection in competitive scenarios
6. **Test with artificial lag** (network simulators) to ensure smooth gameplay at 100-200ms latency

##### Configuration Parameters

```cpp
struct LagConfig {
    uint32_t max_prediction_time_ms = 100;    // How far ahead to predict
    uint32_t interpolation_delay_ms = 100;    // Delay before interpolating
    uint32_t reconciliation_smoothing_ms = 50; // How long to smooth corrections
    float max_position_error = 5.0f;          // Max error before snapping
    bool enable_lag_compensation = true;      // Server-side lag compensation
};
```

---

## Security Considerations

### Packet Validation

All incoming packets must pass strict validation:

```cpp
class PacketValidator {
public:
    enum class ValidationResult {
        VALID,
        INVALID_MAGIC,
        INVALID_VERSION,
        INVALID_SIZE,
        MALFORMED_HEADER,
        MALFORMED_PAYLOAD,
        RATE_LIMITED
    };

    ValidationResult ValidatePacket(const uint8_t* data, size_t size, uint32_t client_id);

private:
    bool ValidateHeader(const PacketHeader& header, size_t total_size);
    bool ValidatePayload(uint8_t message_type, const uint8_t* payload, size_t size);
    bool CheckRateLimit(uint32_t client_id);

    // Rate limiting per client
    std::unordered_map<uint32_t, RateLimiter> client_rate_limits_;
};
```

### Protection Against Common Attacks

#### Buffer Overflow Protection

```cpp
// All message parsing uses safe bounds checking
template<typename T>
bool SafeDeserialize(const uint8_t* data, size_t available_size, T& output) {
    if (available_size < sizeof(T)) {
        return false;  // Not enough data
    }

    // Additional validation for specific message types
    if constexpr (std::is_same_v<T, PlayerInput>) {
        // Validate input ranges
        if (output.movement_x < -1000 || output.movement_x > 1000 ||
            output.movement_y < -1000 || output.movement_y > 1000) {
            return false;
        }
    }

    std::memcpy(&output, data, sizeof(T));
    return true;
}
```

#### Rate Limiting

```cpp
class RateLimiter {
private:
    std::queue<std::chrono::steady_clock::time_point> packet_times_;
    size_t max_packets_per_second_;

public:
    bool AllowPacket() {
        auto now = std::chrono::steady_clock::now();

        // Remove packets older than 1 second
        while (!packet_times_.empty() &&
               now - packet_times_.front() > std::chrono::seconds(1)) {
            packet_times_.pop();
        }

        if (packet_times_.size() >= max_packets_per_second_) {
            return false;  // Rate limit exceeded
        }

        packet_times_.push(now);
        return true;
    }
};
```

#### DDoS Protection

- **Connection limits** per IP address
- **Bandwidth throttling** for excessive senders
- **Packet size limits** to prevent amplification attacks
- **Authentication required** before joining games

### Data Integrity

#### Checksums for Critical Messages

```cpp
// For critical messages, add integrity checksum
struct CriticalMessageHeader {
    uint32_t checksum;    // CRC32 of payload
    uint32_t timestamp;   // Prevent replay attacks
} __attribute__((packed));

uint32_t CalculateChecksum(const uint8_t* data, size_t size) {
    // CRC32 implementation
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < size; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j) {
            crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
        }
    }
    return ~crc;
}
```

---

## Future Extensibility

### Protocol Versioning

The protocol supports seamless version upgrades:

```cpp
class ProtocolVersionManager {
private:
    struct VersionCapabilities {
        uint8_t version;
        std::set<uint8_t> supported_messages;
        std::set<uint8_t> supported_features;
    };

    std::map<uint8_t, VersionCapabilities> supported_versions_;

public:
    bool IsMessageSupported(uint8_t version, uint8_t message_type);
    bool CanCommunicate(uint8_t client_version, uint8_t server_version);
    uint8_t NegotiateVersion(uint8_t client_version);
};
```

### Game-Specific Extensions

New games can be added without modifying the core protocol:

```cpp
// Example: Adding a Pong game
namespace PongProtocol {
    enum PongMessages : uint8_t {
        PADDLE_MOVE = 0xC0,  // Game-specific message range
        BALL_UPDATE = 0xC1,
        SCORE_CHANGE = 0xC2
    };

    struct PaddleMove {
        uint32_t player_id;
        float paddle_y;
        float velocity;
    } __attribute__((packed));

    struct BallUpdate {
        float position_x;
        float position_y;
        float velocity_x;
        float velocity_y;
    } __attribute__((packed));
}
```

### Feature Flags

New features can be enabled selectively:

```cpp
enum class FeatureFlags : uint32_t {
    COMPRESSION     = 0x00000001,
    VOICE_CHAT      = 0x00000002,
    REPLAY_SYSTEM   = 0x00000004,
    SPECTATOR_MODE  = 0x00000008,
    ANTI_CHEAT      = 0x00000010,
    CUSTOM_MAPS     = 0x00000020
};

struct ClientCapabilities {
    uint32_t feature_flags;
    uint32_t max_payload_size;
    uint8_t compression_support;
    uint8_t game_versions[16];  // Supported game versions
} __attribute__((packed));
```

---

## Conclusion

The Keller Protocol Networking Standard (KPN) provides a robust, extensible foundation for multi-game networked applications. Its modular design allows for:

- **Easy integration** of new game types
- **Scalable architecture** supporting multiple concurrent games
- **Advanced networking features** for competitive gaming
- **Forward compatibility** with future protocol enhancements
- **Security** against common network attacks

The protocol is designed to grow with your game engine, supporting everything from simple prototypes to full-featured commercial games while maintaining performance and reliability.

For implementation questions or protocol extensions, refer to the detailed C++ examples provided or consult the game engine documentation.

---

**Document Version**: 1.0
**Last Updated**: September 22, 2025
**Authors**: R-Type Development Team
**Status**: Implementation Ready
