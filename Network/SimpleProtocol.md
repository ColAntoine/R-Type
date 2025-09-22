# KPN Simple Documentation

**Quick Reference for Keller Protocol Networking Standard**

---

## What is KPN?

KPN is the network protocol used by our game engine. It's binary (fast), uses UDP (low latency), and supports multiple games with the same code.

**Key Info:**

- Protocol Magic: `0x504E` ("PN")
- Version: 1
- Transport: UDP (with optional reliability)
- Games Supported: R-Type, future games...

---

## Packet Structure

Every message has the same 8-byte header:

```cpp
struct PacketHeader {
    uint16_t magic;           // 0x504E ("PN")
    uint8_t  version;         // 1
    uint8_t  flags;           // Options (reliable, compressed, etc.)
    uint8_t  message_type;    // What kind of message
    uint8_t  game_id;         // 0=System, 1=R-Type, 2=Pong...
    uint16_t payload_size;    // Data size after header
};
```

**Message Ranges:**

- `0x00-0x3F`: System (connect, disconnect, errors)
- `0x40-0x7F`: Lobby (rooms, matchmaking)
- `0x80-0xBF`: Generic game control
- `0xC0-0xFF`: Game-specific (R-Type entities, etc.)

---

## Essential Messages

### Connection

```cpp
// CLIENT_CONNECT (0x01)
struct ClientConnect {
    uint8_t protocol_version;  // 1
    uint32_t client_version;
    char username[32];
};

// SERVER_ACCEPT (0x02)
struct ServerAccept {
    uint32_t client_id;        // Your assigned ID
    uint32_t server_version;
};

// HEARTBEAT (0x04) - Send every 5 seconds
struct Heartbeat {
    uint32_t timestamp;
    uint16_t client_id;
};
```

### R-Type Game Messages

```cpp
// PLAYER_INPUT (0xC8) - Send user input
struct PlayerInput {
    uint32_t player_id;
    uint32_t timestamp;
    uint8_t input_flags;       // Movement + action buttons
    int16_t movement_x;        // -1000 to +1000
    int16_t movement_y;        // -1000 to +1000
};

// ENTITY_SPAWN (0xC0) - New entity appears
struct EntitySpawn {
    uint32_t entity_id;
    uint8_t entity_type;       // Player, enemy, bullet, etc.
    float position_x;
    float position_y;
    float velocity_x;
    float velocity_y;
};

// ENTITY_UPDATE (0xC1) - Entity moved/changed
struct EntityUpdate {
    uint32_t entity_id;
    uint32_t timestamp;
    float position_x;
    float position_y;
    float velocity_x;
    float velocity_y;
    uint8_t state_flags;       // Health, shields, etc.
};

// ENTITY_DESTROY (0xC2) - Entity removed
struct EntityDestroy {
    uint32_t entity_id;
    uint8_t destruction_type;  // Normal, explosion, fade
    float destruction_x;       // Effect position
    float destruction_y;
};
```

---

## Input Flags (R-Type)

```cpp
enum InputFlags {
    MOVE_UP    = 0x01,
    MOVE_DOWN  = 0x02,
    MOVE_LEFT  = 0x04,
    MOVE_RIGHT = 0x08,
    SHOOT      = 0x10,
    SPECIAL    = 0x20,         // Force command
    FORCE_CMD  = 0x40,
    RESERVED   = 0x80
};
```

---

## Entity Types (R-Type)

```cpp
enum RTypeEntityType {
    PLAYER_SHIP     = 0x01,
    ENEMY_BASIC     = 0x10,
    ENEMY_SNAKE     = 0x11,
    BOSS_DOBKERATOPS = 0x20,
    MISSILE_PLAYER  = 0x30,
    MISSILE_ENEMY   = 0x31,
    FORCE_ORB       = 0x40,
    POWERUP_SPEED   = 0x50,
    POWERUP_MISSILE = 0x51,
    OBSTACLE        = 0x60,
    BACKGROUND_STAR = 0x70
};
```

---

## Reliability

For critical messages (player death, game start, etc.), set the Reliable flag:

```cpp
header.flags = 0x80;  // Reliable bit set
```

The receiver must send back an ACK message (0x06). The sender will resend if no ACK is received within 200ms.

---

## Basic Usage

### Client Game Loop

```cpp
while (game_running) {
    // 1. Send input to server
    PlayerInput input = {player_id, GetTimestamp(), GetInputFlags(), move_x, move_y};
    SendMessage(0xC8, 1, input);  // message_type=0xC8, game_id=1

    // 2. Process server messages
    ProcessIncomingMessages();

    // 3. Render
    RenderGame();

    Sleep(16);  // 60 FPS
}
```

### Server Game Loop

```cpp
while (server_running) {
    // 1. Process client inputs
    ProcessAllClientMessages();

    // 2. Update game world
    UpdateEntities();
    CheckCollisions();

    // 3. Broadcast updates
    for (auto& entity : entities) {
        EntityUpdate update = {entity.id, GetTimestamp(), entity.x, entity.y, entity.vx, entity.vy, entity.flags};
        BroadcastToAll(0xC1, 1, update);  // message_type=0xC1, game_id=1
    }

    Sleep(16);  // 60 FPS
}
```

---

## Safety Rules

### Always Validate Packets

```cpp
bool IsValidPacket(const uint8_t* data, size_t size) {
    if (size < 8) return false;  // Too small for header

    PacketHeader* header = (PacketHeader*)data;
    if (header->magic != 0x504E) return false;  // Wrong protocol
    if (header->version != 1) return false;     // Wrong version
    if (header->payload_size + 8 != size) return false;  // Size mismatch

    return true;
}
```

### Handle Malformed Data

- Never crash on bad packets
- Validate all input ranges (movement -1000 to +1000)
- Limit message rate per client
- Use safe string operations

---

## Advanced Features (Optional)

### Compression

Set compression flag for large packets:

```cpp
header.flags |= 0x40;  // Compressed flag
```

### Fragmentation

Large messages are automatically split into multiple packets.

### Multi-Game Support

Just change the `game_id` field:

- `0x00`: System/Lobby
- `0x01`: R-Type
- `0x02`: Pong
- `0x03`: Your new game

---

## Quick Implementation Checklist

**Phase 1: Basic Connection**

- [ ] Send/receive UDP packets
- [ ] Implement ClientConnect/ServerAccept handshake
- [ ] Add heartbeat system
- [ ] Packet validation

**Phase 2: R-Type Gameplay**

- [ ] PlayerInput messages (movement + shooting)
- [ ] EntitySpawn/Update/Destroy messages
- [ ] Basic collision detection
- [ ] Multiple players

**Phase 3: Polish**

- [ ] Reliable messages for critical events
- [ ] Error handling and reconnection
- [ ] Game lobby system
- [ ] Spectator mode

---

This simplified guide covers 90% of what you need to implement the networking for R-Type. For advanced features, see the full Protocol.md documentation.
