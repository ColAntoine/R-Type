# R-Type - Multiplayer Game

A modern C++ implementation of the classic R-Type space shooter game with real-time multiplayer networking capabilities. Built with a clean, modular architecture featuring an Entity Component System, service-oriented client, and high-performance asynchronous server.

## 🎮 Features

### Core Gameplay
- **Real-time multiplayer**: Multiple players can connect and play simultaneously
- **ECS Architecture**: Entity-Component-System with dynamic library loading
- **State Machine**: Clean game flow (MainMenu → Lobby → WaitingLobby → InGame)
- **UI System**: Component-based UI with buttons, text, input fields
- **Player Control**: Smooth movement with client-side prediction
- **Enemy AI**: Dynamic enemy spawning and behavior patterns
- **Collision System**: Entity collision detection and response
- **Weapon System**: Player shooting with projectile management

### Networking
- **Asynchronous Server**: Asio-based UDP server with thread pool
- **Binary Protocol**: Custom efficient Keller Protocol Networking Standard (KPN)
- **Session Management**: Per-client state tracking and authentication
- **Low Latency**: UDP transport for real-time gameplay
- **Client-Side Prediction**: Responsive controls with server reconciliation
- **Network Synchronization**: Real-time position updates between clients

### Technical Architecture
- **Service-Oriented Design**: Input, Render, Network services with dependency injection
- **Event-Driven Communication**: Decoupled systems via event manager
- **Modular Components**: Clear separation of concerns (ECS, Services, States, Events)
- **Dynamic Libraries**: ECS components and systems loaded at runtime
- **Cross-platform**: Built with CMake and modern C++17
- **Raylib Graphics**: Hardware-accelerated rendering

## 🏗️ Architecture

### Project Structure
```
R-Type/
├── ECS/                          # Entity-Component-System library
│   ├── include/ECS/              # Core ECS headers
│   │   ├── Registry.hpp          # Entity and component management
│   │   ├── Components/           # Base components (position, velocity, sprite, etc.)
│   │   └── Systems/              # Base systems (position, collision, animation, etc.)
│   ├── src/                      # ECS implementation
│   ├── build/                    # Built libraries (libECS.so, system libraries)
│   └── README.md                 # ⭐ ECS technical documentation
│
├── Games/                        # Game implementations
│   ├── RType/                    # R-Type client
│   │   ├── Application.*         # Main application orchestrator
│   │   ├── Core/                 # Core framework
│   │   │   ├── EventManager.*    # Event system
│   │   │   ├── Events.hpp        # Event definitions
│   │   │   ├── Services/         # Service layer (Input, Render, Network)
│   │   │   ├── States/           # State machine (MainMenu, Lobby, InGame, etc.)
│   │   │   └── Systems/          # Core systems (Input, Network)
│   │   ├── Entity/               # Game-specific ECS extensions
│   │   │   ├── Components/       # Game components (Enemy, Health, Weapon, etc.)
│   │   │   └── Systems/          # Game systems (EnemyAI, Shoot, etc.)
│   │   ├── UI/                   # UI system (UIManager, components)
│   │   └── README.md             # ⭐ Client technical documentation
│   └── README.md                 # ⭐ Game development guide
│
├── Network/                      # Server implementation
│   ├── include/                  # Server headers
│   │   ├── network_manager.hpp   # Main network orchestrator
│   │   ├── udp_server.hpp        # Asio-based UDP server
│   │   ├── session.hpp           # Client session management
│   │   ├── message_handler.hpp   # Message routing system
│   │   ├── game_handlers.hpp     # Game message handlers
│   │   └── protocol.hpp          # Binary protocol structures
│   ├── src/                      # Server implementation
│   ├── Protocol.md               # ⭐ Complete protocol specification (KPN v1.0)
│   └── README.md                 # ⭐ Server technical documentation
│
└── CMakeLists.txt                # Root build configuration
```

### Architecture Layers

#### Client Architecture (Games/RType/)
```
┌─────────────────────────────────────┐
│        Application Layer            │
│   (Game Flow, Lifecycle, Main)      │
├─────────────────────────────────────┤
│         State Machine               │
│  (MainMenu, Lobby, InGame, etc.)    │
├─────────────────────────────────────┤
│        Service Layer                │
│    (Input, Render, Network)         │
├─────────────────────────────────────┤
│        Event System                 │
│  (Decoupled Communication)          │
├─────────────────────────────────────┤
│     Entity Component System         │
│  (Game Entities, Components, Logic) │
└─────────────────────────────────────┘
```

#### Server Architecture (Network/)
```
┌─────────────────────────────────────┐
│         Game Logic Layer            │
│    (ECS Systems, Game State)        │
├─────────────────────────────────────┤
│      Message Handlers Layer         │
│  (Connection, Position, Shoot, etc.)│
├─────────────────────────────────────┤
│    Session Management Layer         │
│  (Player State, Authentication)     │
├─────────────────────────────────────┤
│         Protocol Layer              │
│   (Packet Serialization)            │
├─────────────────────────────────────┤
│    Network Transport Layer          │
│   (UDP Server, Asio, Thread Pool)   │
└─────────────────────────────────────┘
```

### Key Technologies
- **ECS**: Custom Entity-Component-System with dynamic library loading
- **Asio**: High-performance asynchronous networking (server)
- **Raylib**: Cross-platform graphics and input (client)
- **CMake**: Build system with vcpkg integration
- **C++17**: Modern C++ with RAII, templates, and smart pointers

## 🚀 Getting Started

### Prerequisites
- **C++17** compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake** 3.10+
- **vcpkg** (handles all dependencies: Raylib, Asio, etc.)

### Quick Build

```bash
# Build all components (vcpkg will automatically install dependencies)
make

# Run server
./r-type_server

# Run client
./r-type_client
```

### Detailed Documentation

For complete technical guides and architecture details:
- **[ECS/README.md](ECS/README.md)** - ECS library API and usage
- **[Games/RType/README.md](Games/RType/README.md)** - Client architecture (Services, States, Events)
- **[Network/README.md](Network/README.md)** - Server architecture (Asio, Threading, Handlers)
- **[Network/Protocol.md](Network/Protocol.md)** - Complete protocol specification (KPN v1.0)
- **[Games/README.md](Games/README.md)** - Game development guide (create your own game)

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

### Building Components

```bash
# Build all components
make                    # Server, client, and ECS library

# Build individual components
cd ECS && ./build.sh    # Build ECS library and systems
cd Network && make      # Build server only
# Client built with root Makefile
```

### Creating New Games

See **[Games/README.md](Games/README.md)** for a complete guide on creating games using the ECS library and following the R-Type architecture pattern.

### Adding Features to R-Type

1. **New Components**: Add to `Games/RType/Entity/Components/`
2. **New Systems**: Add to `Games/RType/Entity/Systems/`
3. **New UI Elements**: Add to `Games/RType/UI/Components/`
4. **New States**: Add to `Games/RType/Core/States/`
5. **New Events**: Add to `Games/RType/Core/Events.hpp`

Refer to **[Games/RType/README.md](Games/RType/README.md)** for detailed client architecture.

### Network Protocol

The server uses the **Keller Protocol Networking Standard (KPN v1.0)**, a custom binary protocol over UDP. See **[Network/Protocol.md](Network/Protocol.md)** for complete specification.

Key message types:
- **CONNECTION_REQUEST** (0x01): Client connects
- **SERVER_ACCEPT** (0x02): Server assigns player ID
- **POSITION_UPDATE** (0xC7): Real-time position sync
- **SHOOT_REQUEST** (0x11): Player shoots
- **DISCONNECT_REQUEST** (0x04): Client disconnects

## � Project Requirements

This project implements the EPITECH R-Type requirements:
- ✅ **Modern Architecture**: ECS library + Service-Oriented client + Async server
- ✅ **UDP Networking**: High-performance real-time multiplayer (KPN protocol)
- ✅ **Binary Protocol**: Custom efficient network protocol with minimal overhead
- ✅ **ECS Pattern**: Entity-Component-System with dynamic library loading
- ✅ **Modular Design**: Clear separation of concerns (ECS/Games/Network)
- ✅ **Cross-platform**: CMake build system with vcpkg integration
- ✅ **Multiplayer**: Simultaneous player support with state synchronization
- ✅ **Extensibility**: Framework for creating new games (see Games/README.md)

## 🤝 Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/new-feature`)
3. Follow existing code style and architecture patterns
4. Update relevant documentation (ECS/, Games/, Network/ READMEs)
5. Commit changes and open a Pull Request

## 📄 License

This project is part of the EPITECH curriculum. Educational use only.
