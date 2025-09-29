# R-Type - Multiplayer Game

A modern C++ implementation of the classic R-Type space shooter game with real-time multiplayer networking capabilities.

## 🎮 Features

### Core Gameplay
- **Real-time multiplayer**: Multiple players can connect and play simultaneously
- **Smooth movement**: Arrow key controls with responsive player movement
- **Visual feedback**: Players appear as red squares, remote players as purple squares
- **Collision system**: Entity collision detection and response
- **ECS Architecture**: Modern Entity-Component-System for game logic

### Networking
- **UDP Protocol**: Fast, real-time networking for smooth gameplay
- **Binary Protocol**: Efficient custom binary protocol for minimal latency
- **Automatic Disconnect Detection**: 10-second timeout for inactive clients
- **Player ID System**: Unique identification for each connected player
- **Position Synchronization**: Real-time position updates between clients

### Technical Features
- **Cross-platform**: Built with CMake and modern C++17
- **Raylib Graphics**: Hardware-accelerated rendering
- **Modular Design**: Separate server and client executables
- **Clean Architecture**: Well-organized codebase with clear separation of concerns

## 🏗️ Architecture

### Project Structure
```
R-Type/
├── Game/                    # Client-side game logic
│   ├── main.cpp            # Client entry point
│   ├── game_engine.*       # Core game logic and ECS integration
│   ├── game_window.*       # Raylib window wrapper
│   └── client.*            # UDP client networking
├── Network/                 # Networking components
│   ├── server.*            # UDP server implementation
│   └── protocol.hpp        # Binary protocol definitions
├── ECS/                    # Entity-Component-System
│   ├── include/ecs/        # ECS headers
│   └── src/                # ECS implementation
└── CMakeLists.txt          # Build configuration
```

### Entity-Component-System (ECS)
- **Components**: `position`, `velocity`, `drawable`, `controllable`, `collider`, `remote_player`
- **Systems**: `position_system`, `control_system`, `draw_system`, `collision_system`
- **Dynamic Loading**: Components loaded via shared libraries

### Network Protocol
Custom binary protocol over UDP for minimal latency:

#### Message Types
- **SERVER_ACCEPT** (0x02): Server assigns player ID to new client
- **PLAYER_DISCONNECT** (0x05): Notify clients when a player leaves
- **ENTITY_UPDATE** (0xC1): Broadcast entity position/state changes
- **POSITION_UPDATE** (0xC7): Direct position synchronization

#### Packet Structure
```cpp
struct PacketHeader {
    uint8_t  message_type;    // Message type identifier
    uint8_t  flags;           // Control flags
    uint16_t payload_size;    // Size of following data
} __attribute__((packed));
```

## 🚀 Getting Started

### Prerequisites
- **C++17** compatible compiler (GCC 8+ or Clang 10+)
- **CMake** 3.10 or higher
- **Raylib** (graphics library)
- **vcpkg** (package manager, optional)

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/ColAntoine/R-Type.git
   cd R-Type
   ```

2. **Install dependencies**
   ```bash
   # Using vcpkg (recommended)
   vcpkg install raylib
   
   # Or install raylib system-wide (Ubuntu/Debian)
   sudo apt-get install libraylib-dev
   ```

3. **Build the project**
   ```bash
   make build
   # or manually:
   mkdir build && cd build
   cmake .. && cmake --build .
   ```

### Running the Game

1. **Start the server**
   ```bash
   cd build
   ./r-type_server
   ```
   Server runs on port 8080 by default.

2. **Start client(s)**
   ```bash
   cd build
   ./r-type_client [server_ip] [server_port]
   # Example: ./r-type_client 127.0.0.1 8080
   ```

### Controls
- **Arrow Keys**: Move your player
- **ESC**: Quit the game

## 🎯 Gameplay

### Multiplayer Experience
- Connect multiple clients to the same server
- Each player appears as a red square on their own screen
- Other players appear as purple squares
- Real-time position synchronization ensures smooth multiplayer
- Players automatically disconnect after 10 seconds of inactivity

### Current Game Elements
- **Player Entity**: Controllable red square (you)
- **Remote Players**: Purple squares (other players)
- **Static Obstacles**: Gray and green walls for collision testing
- **Collision System**: Prevents players from overlapping with obstacles

## 🔧 Development

### Building from Source
```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Release build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Code Structure

#### Server (`Network/server.*`)
- **UDPServer**: Main server class handling client connections
- **ClientInfo**: Stores client connection data and player position
- **Timeout System**: Automatically removes inactive clients (10s timeout)
- **Binary Protocol**: Handles POSITION_UPDATE and ENTITY_UPDATE messages

#### Client (`Game/`)
- **GameEngine**: Core game logic and ECS coordination
- **UDPClient**: Network communication with server
- **GameWindow**: Raylib wrapper for rendering
- **Binary Protocol**: Handles SERVER_ACCEPT, ENTITY_UPDATE, PLAYER_DISCONNECT

#### ECS System (`ECS/`)
- **Registry**: Entity and component management
- **Components**: Position, velocity, drawable, controllable, etc.
- **Systems**: Update logic for movement, rendering, collision
- **Dynamic Loading**: Component factories for extensibility

### Network Flow
1. **Client Connection**: Client sends initial message to server
2. **Server Response**: Server assigns unique player ID via SERVER_ACCEPT
3. **Position Updates**: Client sends POSITION_UPDATE messages (5 FPS)
4. **Entity Broadcasting**: Server broadcasts ENTITY_UPDATE to all other clients
5. **Disconnection**: Server detects timeout and sends PLAYER_DISCONNECT

## 🐛 Known Issues & Limitations

- No persistent player data
- Limited game elements (no enemies, projectiles, or power-ups yet)
- Basic collision system (AABB only)
- No sound effects or background music
- Static obstacles only for testing

## 🔮 Future Enhancements

### Planned Features
- **Enemies & AI**: Various enemy types with different behaviors  
- **Weapons System**: Player projectiles and shooting mechanics
- **Power-ups**: Speed boosts, weapon upgrades, etc.
- **Levels**: Multiple stages with different themes
- **Bosses**: Epic boss battles
- **Sound System**: Music and sound effects
- **Scrolling Background**: Animated starfield
- **Game Modes**: Cooperative, versus, survival modes

### Technical Improvements
- **TCP Option**: Reliable messaging for critical events
- **Lag Compensation**: Client-side prediction and server reconciliation
- **Anti-cheat**: Server-side validation of player actions
- **Scalability**: Support for more concurrent players
- **Persistence**: Player stats and progression saving

## 📋 Requirements (EPITECH R-Type Project)

This project fulfills the following EPITECH requirements:
- ✅ **UDP Networking**: Real-time multiplayer communication
- ✅ **Binary Protocol**: Custom efficient network protocol  
- ✅ **ECS Architecture**: Modern game architecture pattern
- ✅ **Cross-platform**: CMake build system
- ✅ **Multiple Players**: Simultaneous multiplayer support
- ✅ **Real-time Gameplay**: Smooth movement and synchronization
- ✅ **Modular Design**: Clear separation of client/server/ECS

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is part of EPITECH curriculum. Educational use only.

## 👥 Authors

- **ColAntoine** - *Initial work and architecture*

## 🙏 Acknowledgments

- Original R-Type game by Irem
- Raylib community for the graphics library
- EPITECH for the project requirements and guidance