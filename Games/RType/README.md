# R-Type Client Technical Documentation

## Overview
The **R-Type Client** is a networked multiplayer game client built on a modern service-oriented architecture. It combines an Entity Component System (ECS) for game entities, a state machine for game flow, an event-driven communication system, and a service layer for cross-cutting concerns.

### Architecture Principles
1. **Service-Oriented Architecture**:
   - Core services (Input, Rendering, Networking) are managed through dependency injection
   - Services are lifecycle-managed and communicate through events

2. **Event-Driven Design**:
   - Decoupled communication between systems using an event manager
   - Supports both immediate and queued event processing

3. **Entity Component System**:
   - Game entities managed through the ECS framework (see `../../ECS/README.md`)
   - Components define data, systems define behavior

4. **State Machine**:
   - Game flow managed through a state stack
   - States: MainMenu → Lobby → WaitingLobby → InGame
   - Clean state transitions with proper lifecycle management

---

## Project Structure

### Core Architecture

#### 1. `Application.hpp/cpp`
- **Purpose**: Main application orchestrator
- **Responsibilities**:
  - Initializes all core systems (ECS, Services, States, Events)
  - Manages the main game loop
  - Coordinates communication between subsystems
  - Handles application lifecycle (initialize, run, shutdown)
- **Key Components**:
  - `EventManager`: Central event bus for application-wide events
  - `ServiceManager`: Dependency injection container for services
  - `GameStateManager`: State machine for game flow
  - `registry`: ECS registry for game entities
  - `DLLoader`: Dynamic library loader for ECS systems

#### 2. `Core/EventManager.hpp/cpp`
- **Purpose**: Event system backbone
- **Responsibilities**:
  - Type-safe event subscription and emission
  - Supports immediate and queued event processing
  - Decouples system communication
- **Features**:
  - Template-based event handling
  - Event queue for deferred processing
  - Multiple handlers per event type

#### 3. `Core/Events.hpp`
- **Purpose**: Defines all game events
- **Event Categories**:
  - **Input Events**: `KeyPressEvent`, `KeyReleaseEvent`, `MouseMoveEvent`, `MouseButtonEvent`
  - **Player Events**: `PlayerMoveEvent`, `PlayerJoinEvent`, `PlayerLeaveEvent`
  - **Enemy Events**: `EnemySpawnEvent`, `EnemyUpdateEvent`, `EnemyDestroyEvent`
  - **Projectile Events**: `ProjectileSpawnEvent`, `ProjectileDestroyEvent`
  - **Game Events**: `GameStartEvent`, `GameOverEvent`, `ScoreUpdateEvent`
  - **Network Events**: `NetworkConnectedEvent`, `NetworkDisconnectedEvent`, `NetworkErrorEvent`

---

### Service Layer

#### 1. `Core/Services/ServiceManager.hpp/cpp`
- **Purpose**: Dependency injection and service lifecycle manager
- **Responsibilities**:
  - Registers and manages service instances
  - Provides type-safe service retrieval
  - Coordinates service initialization and shutdown
  - Updates all services each frame
- **Pattern**: Service Locator with dependency injection

#### 2. `Core/Services/Input/Input.hpp/cpp`
- **Purpose**: Input service for handling user input
- **Responsibilities**:
  - Captures keyboard and mouse input
  - Emits input events to the event system
  - Maintains input state
- **Integration**: Converts raw input to `KeyPressEvent`, `MouseMoveEvent`, etc.

#### 3. `Core/Services/Render/Render.hpp/cpp`
- **Purpose**: Rendering service using Raylib
- **Responsibilities**:
  - Window management
  - Frame rendering coordination
  - Camera management
  - Debug rendering utilities
- **Technology**: Built on Raylib graphics library

#### 4. `Core/Services/Network/Network.hpp/cpp`
- **Purpose**: Network communication service
- **Responsibilities**:
  - Manages connection to game server (see `../../../Network/`)
  - Sends player actions to server
  - Receives and processes server updates
  - Handles network events (connection, disconnection, errors)
- **Protocol**: Uses UDP for low-latency communication
- **Server Reference**: See `../../../Network/Protocol.md` for protocol details

---

### State Management

#### 1. `Core/States/GameStateManager.hpp/cpp`
- **Purpose**: State machine controller
- **Responsibilities**:
  - Manages state stack (push, pop, change)
  - Handles state lifecycle (enter, exit, pause, resume)
  - Processes pending state operations safely
  - Maintains state factory registry
- **Pattern**: State pattern with stack-based management

#### 2. `Core/States/GameState.hpp`
- **Purpose**: Base interface for all game states
- **Interface**:
  ```cpp
  class IGameState {
      virtual void on_enter() = 0;      // Called when state becomes active
      virtual void on_exit() = 0;       // Called when state is removed
      virtual void on_pause() = 0;      // Called when another state is pushed
      virtual void on_resume() = 0;     // Called when top state is popped
      virtual void update(float dt) = 0; // Called every frame
      virtual void render() = 0;        // Called for rendering
  };
  ```

#### 3. Game States

##### `Core/States/MainMenu/MainMenu.hpp/cpp`
- **Purpose**: Main menu state
- **Features**:
  - Server connection UI (IP/Port input)
  - Player name input
  - Play button to enter lobby
  - Settings and quit options
- **Transition**: MainMenu → Lobby (on Play)

##### `Core/States/Lobby/Lobby.hpp/cpp`
- **Purpose**: Room selection and creation
- **Features**:
  - Display available game rooms
  - Create new room button
  - Join existing room
  - Refresh room list
- **Transition**: Lobby → WaitingLobby (on room join/create)

##### `Core/States/WaitingLobby/WaitingLobby.hpp/cpp`
- **Purpose**: Pre-game waiting room
- **Features**:
  - Display connected players
  - Ready/Unready system
  - Room settings display
  - Leave room button
- **Transition**: WaitingLobby → InGame (when all ready)

##### `Core/States/InGame/InGame.hpp/cpp`
- **Purpose**: Main gameplay state
- **Features**:
  - Active gameplay
  - ECS system updates
  - Player control
  - Enemy spawning and AI
  - Collision detection
  - Score tracking
  - Network synchronization
- **Transition**: InGame → MainMenu (on game over/quit)

##### `Core/States/Loading/Loading.hpp/cpp`
- **Purpose**: Asset loading screen
- **Features**:
  - Loading progress display
  - Asset preloading
  - Smooth transition to next state

---

### Network Integration

#### 1. `Core/Client/Client.hpp/cpp` (UDPClient)
- **Purpose**: Low-level UDP network client
- **Responsibilities**:
  - Establishes UDP connection to server
  - Sends packets to server
  - Receives packets from server (separate thread)
  - Manages connection state
- **Threading**: Dedicated receive thread for non-blocking I/O
- **Server Location**: See `../../../Network/` for server implementation

#### 2. `Core/Systems/Network/Network.hpp/cpp`
- **Purpose**: High-level network system
- **Responsibilities**:
  - Serializes game state to network messages
  - Deserializes server updates to game events
  - Handles protocol-level communication
  - Manages client-side prediction and reconciliation
- **Integration**: Bridges between ECS/Events and raw network packets

---

### Entity Management

#### Components (`Entity/Components/`)
Game-specific components extending the base ECS:

- **`Controllable/`**: Marks entities that can be controlled by local player
- **`Drawable/`**: Contains rendering information (sprite, texture, color)
- **`Enemy/`**: Enemy-specific data (type, behavior, AI state)
- **`Health/`**: Health points and damage tracking
- **`LifeTime/`**: Time-limited entities (projectiles, effects)
- **`NetworkSync/`**: Network synchronization data (entity ID, owner)
- **`Projectile/`**: Projectile-specific data (damage, speed, owner)
- **`RemotePlayer/`**: Remote player entity data
- **`Score/`**: Score tracking component
- **`Spawner/`**: Entity spawner data (spawn rate, type)
- **`Weapon/`**: Weapon system data (cooldown, projectile type)

#### Systems (`Entity/Systems/`)
Game-specific systems extending the base ECS:

- **`Control/`**: Handles player input and translates to entity movement
- **`Draw/`**: Renders all drawable entities to screen
- **`EnemyAI/`**: Enemy behavior and movement patterns
- **`EnemyCleanup/`**: Removes destroyed enemies from ECS
- **`EnemySpawnSystem/`**: Spawns enemies based on game state
- **`Health/`**: Processes damage and health-related logic
- **`LifeTime/`**: Destroys entities after their lifetime expires
- **`NetworkSyncSystem/`**: Synchronizes remote entities with server state
- **`Shoot/`**: Handles weapon firing and projectile creation
- **`Spawn/`**: Generic entity spawning system

---

### User Interface

#### 1. `UI/UIManager.hpp/cpp`
- **Purpose**: UI component orchestrator
- **Responsibilities**:
  - Manages all UI components
  - Handles UI input (mouse clicks, keyboard)
  - Renders UI elements
  - Component lifecycle management
- **Pattern**: Composite pattern with named component registry

#### 2. `UI/UIComponent.hpp`
- **Purpose**: Base interface for UI components
- **Interface**:
  ```cpp
  class IUIComponent {
      virtual void update(float dt) = 0;
      virtual void render() = 0;
      virtual bool handle_input() = 0;  // Returns true if input consumed
      virtual bool is_mouse_over(float x, float y) = 0;
  };
  ```

#### 3. UI Components (`UI/Components/`)

##### `UIButton.hpp/cpp`
- **Purpose**: Interactive button component
- **Features**:
  - Click callbacks
  - Hover states
  - Customizable appearance
  - Text label support

##### `UIText.hpp/cpp`
- **Purpose**: Text display component
- **Features**:
  - Multi-line text support
  - Text alignment
  - Font customization
  - Color and style options

##### `UIInputField.hpp/cpp`
- **Purpose**: Text input component
- **Features**:
  - Keyboard input capture
  - Text validation
  - Placeholder text
  - Password masking
  - Character limits

##### `UIPanel.hpp/cpp`
- **Purpose**: Container component for grouping UI elements
- **Features**:
  - Background rendering
  - Border support
  - Child component management
  - Layout positioning

---

## Game Flow

### 1. Application Startup
```
main() 
  → Application::initialize()
    → Load ECS components library (libECS.so)
    → Load ECS systems (Control, Draw, Health, etc.)
    → Register services (Input, Render, Network)
    → Initialize EventManager
    → Register game states (MainMenu, Lobby, InGame, etc.)
    → Push MainMenu state
  → Application::run()
    → Main game loop
```

### 2. Main Game Loop
```
while (running) {
    1. Handle Window Events
    2. Update Services (Input, Network)
    3. Process Event Queue
    4. Update Active State
    5. Update ECS Systems
    6. Render State
    7. Render UI
    8. Frame Timing
}
```

### 3. State Transition Example: Joining Game
```
MainMenu (User enters IP, clicks Play)
  → Emit NetworkConnectEvent
  → Network Service connects to server
  → On connection success: Push Lobby state
    
Lobby (User clicks "Join Room")
  → Send JOIN_ROOM message to server
  → On success: Push WaitingLobby state
  
WaitingLobby (All players ready)
  → Server sends GAME_START message
  → Change to InGame state
  → Load player entities
  → Start game systems
```

### 4. Network Communication Flow
```
Client Action (e.g., Player moves)
  → Control System detects input
  → Updates local entity (client-side prediction)
  → Emit PlayerMoveEvent
  → Network System serializes event
  → Send packet to server (see Network/Protocol.md)
  
Server Update Received
  → Network Service receives packet
  → Network System deserializes
  → Emit appropriate event (PlayerMoveEvent, EnemySpawnEvent, etc.)
  → Systems react to events
  → Update ECS entities
```

---

## Integration with ECS Library (short)

The R-Type client uses the shared ECS library for entity management. For core ECS concepts, component definitions and usage examples please refer to `../../ECS/README.md`.

In short:
- The client loads the ECS components library at startup and uses `DLLoader` to load game systems when required.
- Game-specific components and systems live under `Entity/Components/` and `Entity/Systems/`.


---

## Network Protocol

The client communicates with the server using a UDP-based protocol. For detailed protocol specification, see `../../../Network/Protocol.md`.

### Key Message Types

#### Client → Server
- **CONNECT**: Initial connection request with player name
- **DISCONNECT**: Graceful disconnection
- **INPUT**: Player input state (movement, shooting)
- **READY**: Player ready in lobby
- **JOIN_ROOM**: Request to join a game room
- **CREATE_ROOM**: Request to create a new room

#### Server → Client
- **PLAYER_JOIN**: New player connected
- **PLAYER_LEAVE**: Player disconnected
- **PLAYER_UPDATE**: Player position/state update
- **ENEMY_SPAWN**: New enemy spawned
- **ENEMY_UPDATE**: Enemy position/state update
- **ENEMY_DESTROY**: Enemy destroyed
- **PROJECTILE_SPAWN**: New projectile created
- **PROJECTILE_DESTROY**: Projectile destroyed
- **GAME_START**: Game session starting
- **GAME_OVER**: Game session ended
- **SCORE_UPDATE**: Player score changed

### Network Synchronization Strategy

1. **Client-Side Prediction**:
   - Local player input is applied immediately to local entity
   - Prevents input lag for responsive gameplay

2. **Server Reconciliation**:
   - Server authoritative for all entity states
   - Server updates override client predictions
   - Smooth interpolation to hide corrections

3. **Entity Interpolation**:
   - Remote entities (other players, enemies) are interpolated
   - Reduces jitter from network updates
   - Maintains smooth visual movement

---

## Event-Driven Communication

### Event Flow Example: Player Shoots

```
1. User presses SPACE
   → Input Service detects key press
   → Emit KeyPressEvent(KEY_SPACE)

2. Control System receives KeyPressEvent
   → Check if entity has weapon component
   → Check weapon cooldown
   → Create projectile entity
   → Emit ProjectileSpawnEvent

3. Network System receives ProjectileSpawnEvent
   → Serialize projectile data
   → Send SHOOT packet to server

4. Server validates and broadcasts
   → All clients receive PROJECTILE_SPAWN
   → Network System deserializes
   → Emit ProjectileSpawnEvent
   → Systems create/update projectile entities
```

---

## Building and Running

### Prerequisites
- C++17 compiler
- CMake 3.10+
- Raylib (graphics library)
- ECS library built (`../../ECS/libECS.so`)
- Network server running (see `../../../Network/README.md`)

### Build Instructions
```bash
# From R-Type root directory
cd Games/RType
mkdir build && cd build
cmake ..
make

# Run client
./r-type_client
```

### Configuration
- Default server: `127.0.0.1:8080`
- Configurable through Main Menu UI
- Connection settings not persisted between sessions

---

## Code Examples

### Adding a New Game State

```cpp
// 1. Create state class
class NewState : public IGameState {
public:
    void on_enter() override {
        // Initialize state resources
    }
    
    void on_exit() override {
        // Cleanup state resources
    }
    
    void update(float dt) override {
        // Update logic
    }
    
    void render() override {
        // Render logic
    }
    
    void on_pause() override {}
    void on_resume() override {}
};

// 2. Register state in Application::initialize()
_stateManager.register_state<NewState>("NewState");

// 3. Transition to state
_stateManager.push_state("NewState");
```

### Creating a Custom Event

```cpp
// 1. Define event in Events.hpp
struct CustomEvent : Event {
    int custom_data;
    std::string message;
    CustomEvent(int data, const std::string& msg) 
        : custom_data(data), message(msg) {}
};

// 2. Subscribe to event
event_manager_.subscribe<CustomEvent>([](const CustomEvent& e) {
    std::cout << "Received: " << e.message << std::endl;
});

// 3. Emit event
event_manager_.emit(CustomEvent(42, "Hello World"));
```

### Adding a New Service

```cpp
// 1. Create service implementing IService
class AudioService : public IService {
public:
    void initialize() override {
        // Load audio system
    }
    
    void shutdown() override {
        // Cleanup audio system
    }
    
    void update(float delta_time) override {
        // Update audio (if needed)
    }
    
    void play_sound(const std::string& sound_id) {
        // Play sound implementation
    }
};

// 2. Register service in Application::initialize()
service_manager_.register_service<AudioService>();

// 3. Use service anywhere
auto& audio = service_manager_.get_service<AudioService>();
audio.play_sound("explosion");
```

---

## Debugging and Development

### Debug Features
- **Event Logging**: Enable verbose event logging in EventManager
- **Network Packet Inspection**: Log all sent/received packets
- **ECS Entity Viewer**: Display all entities and their components
- **State Stack Viewer**: Show current state stack

### Common Issues

#### Connection Failed
- Verify server is running (`../../../Network/r-type_server`)
- Check IP address and port
- Verify firewall settings
- Check server logs for connection attempts

#### Input Not Working
- Ensure Input Service is initialized
- Check event subscriptions
- Verify Control System is loaded
- Check if UI is consuming input

#### Entities Not Rendering
- Verify sprite component has valid texture path
- Check if Draw System is loaded and updating
- Ensure Render Service is initialized
- Check camera positioning

#### Network Desync
- Check server authority implementation
- Verify network interpolation settings
- Review client-side prediction logic
- Check packet loss and latency

---

## Performance Considerations

### Optimization Strategies

1. **ECS Performance**:
   - Systems iterate only over entities with required components
   - Cache-friendly data layout via sparse sets
   - Minimize component additions/removals during gameplay

2. **Network Performance**:
   - UDP for low latency
   - Client-side prediction reduces perceived lag
   - State delta compression (only send changes)
   - Interest management (only sync relevant entities)

3. **Rendering Performance**:
   - Batch draw calls by texture
   - Frustum culling for off-screen entities
   - Sprite atlases to reduce texture switches
   - LOD for distant entities

4. **Event Performance**:
   - Immediate mode for time-critical events
   - Queued mode for batch processing
   - Event handler caching
   - Minimize event payload size

---

## Architecture Advantages

1. **Modularity**:
   - Services are independent and interchangeable
   - States encapsulate distinct game phases
   - ECS separates data from behavior

2. **Testability**:
   - Services can be mocked for unit tests
   - Event-driven design enables integration testing
   - State transitions are deterministic

3. **Scalability**:
   - Easy to add new game states
   - Simple to extend with new components/systems
   - Services can be enhanced without affecting others

4. **Maintainability**:
   - Clear separation of concerns
   - Event-driven reduces coupling
   - State pattern organizes game flow

---

## Future Improvements

1. **Gameplay**:
   - Power-ups and weapon upgrades
   - Boss battles with complex patterns
   - Multiplayer co-op mode
   - Leaderboards and statistics

2. **Technical**:
   - Asset hot-reloading for faster iteration
   - Replay system for debugging and sharing
   - Server-side anti-cheat validation
   - WebSocket fallback for restricted networks

3. **User Experience**:
   - Rebindable controls
   - Graphics settings menu
   - Audio settings and music
   - Tutorial and help screens

4. **Network**:
   - Lag compensation improvements
   - Server browser with filters
   - Friend system and invites
   - Reconnection handling

---

## Contributing

Contributions are welcome! Please follow these guidelines:

1. **Code Style**:
   - Follow existing code conventions
   - Use meaningful variable names
   - Add comments for complex logic

2. **Architecture**:
   - New features should fit the service-oriented design
   - Use events for cross-system communication
   - Add new game logic as ECS systems when possible

3. **Testing**:
   - Test with both local and remote servers
   - Verify multiplayer synchronization
   - Check edge cases (disconnect, lag, etc.)

4. **Documentation**:
   - Update this README for architectural changes
   - Document new events in Events.hpp
   - Add comments to public APIs

---

## Related Documentation

- **ECS Library**: `../../ECS/README.md` - Core entity management system
- **Network Protocol**: `../../../Network/Protocol.md` - Client-server communication spec
- **Server Architecture**: `../../../Network/README_NEW_ARCHITECTURE.md` - Server implementation
- **Build System**: `../../README.md` - Project-wide build instructions

---

## License

This project is part of the R-Type project at EPITECH.

---

## Authors

**R-Type Development Team**

- Project Repository: [R-Type](https://github.com/ColAntoine/R-Type)
- Organization: EPITECH

---

## Acknowledgments

- Inspired by classic R-Type arcade game
- Built with modern C++ and software engineering practices
- Service-oriented architecture for maintainability and scalability
- Event-driven design for decoupled communication

---

<div align="center">
  <strong>Built for multiplayer gaming excellence</strong>
  <br>
  <sub>© 2025 R-Type Team - EPITECH</sub>
</div>
