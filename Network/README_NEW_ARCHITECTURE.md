# R-Type Network Architecture

## Overview

This is a complete refactor of the R-Type network system using modern C++ and Asio for high-performance, scalable networking.

## Architecture

### Core Components

1. **NetworkManager** - Main orchestrator
   - Manages io_context and worker threads
   - Provides clean startup/shutdown interface
   - Handles thread pool management

2. **UdpServer** - Asio-based UDP server
   - Asynchronous message handling
   - Client session management
   - Automatic cleanup of disconnected clients

3. **Session** - Per-client state management
   - Player information and authentication
   - Position tracking
   - Connection lifecycle management

4. **Connection** - Network connection abstraction
   - Endpoint management
   - Send operations (sync/async)
   - Activity tracking

5. **MessageHandler System** - Clean message dispatch
   - Type-safe message handling
   - Pluggable handler architecture
   - Easy to extend with new message types

6. **Protocol** - Improved message format
   - Structured packet format with headers
   - Type-safe serialization/deserialization
   - Validation and error handling

## Key Improvements

### From Old Architecture
- **Modular Design**: No more 476-line single file
- **Asio Integration**: Modern async I/O instead of raw sockets
- **Type Safety**: Structured message handling vs raw buffer manipulation
- **Scalability**: Thread pool vs single-threaded blocking I/O
- **Maintainability**: Clear separation of concerns
- **Testability**: Injectable dependencies and interfaces

### Performance Benefits
- Non-blocking I/O operations
- Efficient thread pool utilization
- Automatic connection cleanup
- Reduced memory allocations
- Better error handling

## Building

### Prerequisites
- C++17 compatible compiler
- CMake 3.16+
- Either:
  - Standalone Asio library, OR
  - Boost.Asio (Boost 1.70+)

### Linux/macOS
```bash
# Install standalone Asio (recommended)
# Ubuntu/Debian:
sudo apt install libasio-dev

# macOS with Homebrew:
brew install asio

# Or install Boost.Asio
# Ubuntu/Debian:
sudo apt install libboost-system-dev

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Running the Server
```bash
# Default port (8080)
./rtype_server

# Custom port
./rtype_server 12345
```

## Usage

### Server Commands
- `info` - Show server status and client count
- `clients` - List connected clients  
- `broadcast <message>` - Send message to all clients
- `quit` - Gracefully shutdown server

### Client Integration
```cpp
#include "network_manager.hpp"
#include "protocol.hpp"

// Create network manager
RType::Network::NetworkManager network;
network.initialize(8080);
network.start();

// Send position update
RType::Protocol::PositionUpdate pos;
pos.entity_id = player_id;
pos.x = 100.0f;
pos.y = 200.0f;
pos.timestamp = get_timestamp();

auto packet = RType::Protocol::create_packet(
    static_cast<uint8_t>(RType::Protocol::GameMessage::POSITION_UPDATE), 
    pos
);
```

## Message Types

### System Messages
- `CLIENT_CONNECT` - Client connection request
- `SERVER_ACCEPT` - Server acceptance response  
- `PING/PONG` - Keep-alive and latency measurement
- `CLIENT_DISCONNECT` - Graceful disconnection

### Game Messages
- `POSITION_UPDATE` - Player/entity position updates
- `PLAYER_SHOOT` - Player shooting actions
- `ENTITY_CREATE/DESTROY` - Entity lifecycle
- `GAME_START/END` - Game state changes

## Adding New Message Types

1. **Define message structure** in `protocol.hpp`:
```cpp
struct NewMessage {
    uint32_t data1;
    float data2;
} __attribute__((packed));
```

2. **Add message type enum**:
```cpp
enum class GameMessage : uint8_t {
    // ... existing types
    NEW_MESSAGE = 0xD5,
};
```

3. **Create message handler**:
```cpp
class NewMessageHandler : public TypedMessageHandler<NewMessage> {
    uint8_t get_message_type() const override {
        return static_cast<uint8_t>(GameMessage::NEW_MESSAGE);
    }
    
protected:
    bool handle_typed_message(/*...*/) override {
        // Handle message logic
        return true;
    }
};
```

4. **Register handler** in `main.cpp`:
```cpp
dispatcher->register_handler(
    static_cast<uint8_t>(GameMessage::NEW_MESSAGE),
    std::make_shared<NewMessageHandler>()
);
```

## Testing

### Unit Tests (Coming Soon)
```bash
# Build with tests
cmake -DBUILD_TESTS=ON ..
make test
```

### Manual Testing
Use netcat to send raw UDP packets:
```bash
# Send test packet
echo -e "\\x01\\x00\\x00\\x20test_player_name\\x00\\x00\\x01\\x00\\x00\\x00" | nc -u localhost 8080
```

## Migration from Old System

The old system has been preserved with `_old` suffix:
- `server_old.hpp`
- `server_old.cpp`
- `main_old.cpp`

### Key Migration Points
1. Replace `UDPServer` with `NetworkManager`
2. Update message structures to use new protocol
3. Replace direct socket calls with session/connection methods
4. Update message handling to use dispatcher system

## Performance Tuning

### Thread Configuration
```cpp
// Use hardware concurrency
size_t threads = std::thread::hardware_concurrency();

// Or customize
size_t threads = 4; // For 4-core systems
network.start(threads);
```

### Buffer Sizes
Adjust in `protocol.hpp`:
```cpp
constexpr size_t MAX_PACKET_SIZE = 2048; // Increase for larger messages
```

### Cleanup Frequency
Modify timer in `UdpServer::setup_cleanup_timer()`:
```cpp
cleanup_timer_.expires_after(std::chrono::seconds(10)); // More frequent cleanup
```

## Future Enhancements

- [ ] TCP support for reliable messages
- [ ] Message compression
- [ ] Connection pooling
- [ ] Metrics and monitoring
- [ ] SSL/TLS encryption
- [ ] Message fragmentation for large packets
- [ ] Client-side networking library
- [ ] Load balancing support

## Troubleshooting

### Common Issues

1. **"Address already in use"**
   ```bash
   # Wait or use different port
   ./rtype_server 8081
   ```

2. **"Asio not found"**
   ```bash
   # Install dependencies
   sudo apt install libasio-dev  # or libboost-system-dev
   ```

3. **High CPU usage**
   - Reduce worker thread count
   - Increase cleanup timer interval
   - Check for message handler infinite loops

### Debug Build
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
gdb ./rtype_server
```

## Contributing

1. Follow the existing code style
2. Add unit tests for new features
3. Update documentation
4. Test with multiple clients
5. Profile performance impact