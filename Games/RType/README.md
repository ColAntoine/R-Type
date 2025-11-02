# R-Type: Multiplayer Space Shooter

Welcome to **R-Type**, a modern implementation of the classic arcade space shooter with real-time multiplayer support. Battle across waves of enemies, dodge projectiles, and survive with your friends in this intense action game!

---

## 📖 Table of Contents

- [For Players](#-for-players)
  - [Getting Started](#getting-started)
  - [How to Play](#how-to-play)
  - [Gameplay Features](#gameplay-features)
  - [Controls](#-controls)
  - [Multiplayer Guide](#multiplayer-guide)
  - [Troubleshooting](#troubleshooting)
- [For Developers](#-for-developers)
  - [Architecture Overview](#architecture-overview)
  - [Project Structure](#project-structure)
  - [Setting Up Development Environment](#setting-up-development-environment)
  - [Building from Source](#building-from-source)
  - [Understanding the Code](#understanding-the-code)
  - [Adding New Features](#adding-new-features)
  - [Creating New Scenes](#creating-new-scenes)
  - [Creating Plugins/Mods](#creating-pluginsmods)

---

# 🎮 For Players

## Getting Started

### Prerequisites
- Windows 10+, macOS 10.14+, or Linux (Ubuntu 18.04+)
- A stable internet connection for multiplayer

### Installation

1. **Download the latest release** from the [R-Type releases page](https://github.com/ColAntoine/R-Type/releases)
2. **Extract the archive** to your preferred location
3. **Find the game executable**: `r-type_client`

### Quick Start - Play the Game

Simply run the game client:
```bash
./r-type_client
```

The game will launch and automatically connect to the default server. You can then join a lobby and start playing!

---

## How to Play

When you launch the game, you'll enter the **Main Menu** where you can:
- **Play** — Start a new game
- **Settings** — Adjust graphics, audio, and key bindings
- **Quit** — Exit the game

From the main menu, you can join a lobby with other players and start playing together. Battle through waves of enemies, collect upgrades, and try to survive as long as possible!

---

## Gameplay Features

### Wave System
- Enemies spawn in organized **waves**
- Each wave increases in difficulty
- Survive all waves to complete a level

### Enemy Types
- **Basic** — Standard enemies moving straight at you
- **Sine Wave** — Weaving back and forth while approaching
- **Fast** — High-speed attackers moving quickly
- **Zigzag** — Erratic movement pattern, hard to predict
- **Turret** — Stationary enemies that fire from distance
- **Boss** — Powerful enemies with high health and special attacks

### Weapons & Upgrades
- **Hard Bullet** — Standard rapid-fire projectiles
- **Big Bullet** — Heavy projectiles with increased damage (2x)
- **Parabol Shot** — Projectiles with gravity, arc trajectory
- **Explosion** — Burst of projectiles in all directions
- **Speed Boost** — Increase your movement speed
- **Fire Rate Boost** — Shoot more rapidly
- **Damage Upgrade** — Increase projectile damage

### Collision & Physics
- Collide with obstacles to take damage
- Survive longer = higher score
- Use cover to protect yourself

---

## 🎮 Controls

| Action | Key |
|--------|-----|
| Move Up | **W** |
| Move Down | **S** |
| Move Left | **A** |
| Move Right | **D** |
| Shoot | **SPACE** |
| Pause/Menu | **ESC** |

---

## Multiplayer Guide

### How It Works

R-Type is a multiplayer-first game. When you launch the game, you automatically connect to the server. Simply join a lobby to play with others.

**Features:**
- **Real-time multiplayer** — Play with 2-8 players simultaneously
- **Smooth networking** — Client-side prediction and interpolation minimize lag
- **Shared progression** — All players face the same waves of enemies

### Network Performance

R-Type uses advanced networking techniques to ensure smooth gameplay:
- Your controls are instantly responsive (client-side prediction)
- Other players' positions are smoothly interpolated
- Server corrections are subtle and unnoticeable

**Recommended:**
- **Ping**: < 150ms for best experience
- **Connection**: Stable, low packet loss

If you experience lag, try:
1. Checking your internet connection
2. Lowering graphics settings
3. Closing other bandwidth-heavy applications

---

## Troubleshooting

### Game Won't Start

**Problem**: Game crashes on startup
- Update your graphics drivers
- Lower graphics quality in settings
- Verify game files are not corrupted

### Can't Connect to Server

**Problem**: "Connection refused" error
- Check your internet connection
- Verify you can reach the server
- Try restarting the game

### Game Runs Slowly

- Lower graphics settings (resolution, scale)
- Close other applications in background
- Check CPU/GPU temperatures
- Update your graphics drivers

### Multiplayer Issues

**Players teleporting or desynchronizing**:
- Restart the game
- Check your internet connection for packet loss

**High ping/latency**:
- Close bandwidth-heavy applications
- Try connecting via a wired connection instead of WiFi

---

# 🛠️ For Developers

## Architecture Overview

R-Type is built with a **modular, service-oriented architecture**:

```
┌──────────────────────────────────────────────┐
│         Application Layer (Main Loop)        │
├──────────────────────────────────────────────┤
│       State Machine (Menus, Lobbies, Game)   │
├──────────────────────────────────────────────┤
│   Service Layer (Input, Render, Network)     │
├──────────────────────────────────────────────┤
│      Event System (Decoupled Messaging)      │
├──────────────────────────────────────────────┤
│  Entity Component System (ECS) - Game Logic  │
│            (Powered by ECS Library)          │
├──────────────────────────────────────────────┤
│    Rendering Engine (Sprite Batch, UI)       │
├──────────────────────────────────────────────┤
│  Networking Client (UDP, Protocol Handling)  │
└──────────────────────────────────────────────┘
```

### Key Design Principles

- **Separation of Concerns** — Each module has a single responsibility
- **Dependency Injection** — Services are injected, not global
- **Event-Driven** — Systems communicate via events, not direct calls
- **ECS-Based** — Game logic uses Entity-Component-System pattern
- **Extensible** — Easy to add new features without modifying core code

---

## Project Structure

```
Games/RType/
├── main_client.cpp                 # Entry point for the game client
├── Application.hpp/cpp             # Main application orchestrator
├── Core/
│   ├── EventManager.hpp/cpp        # Event bus for inter-system communication
│   ├── Events.hpp                  # Event type definitions
│   ├── Services/
│   │   ├── IService.hpp            # Service base interface
│   │   ├── InputService.hpp/cpp    # Keyboard/mouse input handling
│   │   ├── RenderService.hpp/cpp   # Rendering orchestration
│   │   └── NetworkService.hpp/cpp  # Network client (send/receive)
│   ├── States/
│   │   ├── IGameState.hpp          # State machine interface
│   │   ├── MainMenuState.hpp/cpp   # Main menu
│   │   ├── LobbyState.hpp/cpp      # Lobby/matchmaking
│   │   ├── GameState.hpp/cpp       # In-game play state
│   │   └── PauseState.hpp/cpp      # Pause menu
│   └── Systems/
│       ├── InputSystem.hpp/cpp     # Processes keyboard input
│       └── NetworkSystem.hpp/cpp   # Sends network updates
│
├── Entity/
│   ├── Components/                 # Game-specific components
│   │   ├── Health.hpp              # Health/shield system
│   │   ├── Weapon.hpp              # Player weapons
│   │   ├── Enemy.hpp               # Enemy behavior data
│   │   ├── Movement.hpp            # Speed/direction modifiers
│   │   └── ...more components
│   └── Systems/                    # Game-specific systems
│       ├── EnemyAISystem.hpp       # Enemy behavior logic
│       ├── WeaponSystem.hpp        # Weapon firing/projectiles
│       ├── HealthSystem.hpp        # Damage/death handling
│       └── ...more systems
│
├── UI/
│   ├── UIManager.hpp/cpp           # UI orchestration
│   ├── Components/
│   │   ├── Button.hpp              # Clickable buttons
│   │   ├── Label.hpp               # Text display
│   │   ├── ProgressBar.hpp         # Health bars
│   │   └── ...more UI components
│   └── Screens/
│       ├── MainMenuScreen.hpp      # Menu UI layout
│       ├── HUDScreen.hpp           # In-game HUD
│       └── PauseScreen.hpp         # Pause menu UI
│
├── Assets/
│   ├── Textures/
│   │   ├── player.png
│   │   ├── enemies.png
│   │   └── ...sprites
│   ├── Sounds/
│   │   ├── shoot.wav
│   │   ├── explosion.wav
│   │   └── ...audio
│   └── Fonts/
│       └── arial.ttf
│
├── Networking.md                   # Detailed networking architecture
├── README.md                       # This file
├── Constants.hpp                   # Game constants/configuration
└── build.sh                        # Build script for the game
```

### Directory Organization

**Core/** — Framework & infrastructure (reusable across games)
- States, Services, Events — Game flow management
- Designed to be extended, not modified

**Entity/** — Game-specific logic (unique to R-Type)
- Components & Systems for R-Type mechanics
- Where game-specific features are added

**UI/** — User interface (game-specific)
- Screens, layouts, interactive elements

---

## Setting Up Development Environment

### Prerequisites

- **C++17 compatible compiler**:
  - GCC 7+ (Linux)
  - Clang 5+ (macOS/Linux)
  - MSVC 2017+ (Windows)
- **CMake 3.10+** — Build system
- **Git** — Version control
- **vcpkg** — Package manager (dependency management)

### Installing Dependencies

```bash
# 1. Clone the repository
git clone https://github.com/ColAntoine/R-Type.git
cd R-Type

# 2. Initialize vcpkg (handles all C++ dependencies)
./external/vcpkg/bootstrap-vcpkg.sh  # Linux/macOS
# or
.\external\vcpkg\bootstrap-vcpkg.bat  # Windows

# 3. Dependencies are automatically installed by CMake
```

### IDE Setup

#### Visual Studio Code
1. Install extensions:
   - C/C++ (Microsoft)
   - CMake Tools (Microsoft)
   - Better C++ Syntax

2. Open the workspace:
   ```bash
   code .
   ```

3. Select CMake kit (compiler) when prompted

#### Visual Studio (Windows)
1. Open CMake as a project:
   - File → Open → CMake
   - Select `CMakeLists.txt`

2. Build via the CMake menu

#### CLion (JetBrains)
1. Open the project
2. CLion auto-detects CMake and vcpkg
3. Build & run from the IDE

---

## Building from Source

### Quick Build

```bash
# Build everything (server, client, ECS)
make

# Or manually:
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### Build Artifacts

After building, you'll find:
```
build/
├── r-type_client          # Game executable
├── r-type_server          # Server executable
├── lib/
│   ├── libECS.so          # ECS library
│   └── systems/           # Dynamically loaded systems
└── Games/RType/
    └── Assets/            # Game resources (copied automatically)
```

### Build Options

```bash
# Debug build (with symbols, slower)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release build (optimized, faster)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build specific target
cmake --build . --target r-type_client
```

### Rebuilding After Changes

```bash
# Quick rebuild (incremental)
cd build
cmake --build .

# Full clean rebuild
cd build
cmake --build . --target clean
cmake --build .
```

---

## Understanding the Code

### Code Flow: Game Startup

```cpp
// main_client.cpp
int main(int argc, char* argv[]) {
    // 1. Parse arguments (server IP, port)
    std::string server_ip = argv[1];    // e.g., "127.0.0.1"
    int server_port = std::stoi(argv[2]); // e.g., 8080
    
    // 2. Initialize services
    InputService input_service;
    RenderService render_service;
    NetworkService network_service(server_ip, server_port);
    
    // 3. Create event manager
    EventManager event_manager;
    
    // 4. Create application
    Application app(event_manager, input_service, render_service, network_service);
    
    // 5. Run main loop
    app.run();  // Blocks until game exits
}
```

### Code Flow: Main Loop

```cpp
// Application.cpp
void Application::run() {
    while (is_running) {
        // 1. Handle input
        input_service.update();  // Capture keyboard/mouse
        
        // 2. Process network
        network_service.receive_updates();  // Get server updates
        
        // 3. Update game state
        current_state->update(dt);  // Process game logic
        
        // 4. Render frame
        render_service.begin_frame();
            current_state->render();  // Draw everything
        render_service.end_frame();   // Swap buffers
        
        // 5. Emit events
        event_manager.emit_all();     // Process queued events
    }
}
```

### Event System

Events allow loose coupling between systems:

```cpp
// Subscribe to events (e.g., in GameState constructor)
event_manager.subscribe("PLAYER_SHOT", [this](const Event& e) {
    int weapon_type = e.get<int>("weapon");
    spawn_projectile(weapon_type);
});

// Emit event (e.g., in InputSystem)
if (input_service.is_key_pressed(KEY_SPACE)) {
    Event shoot_event("PLAYER_SHOT");
    shoot_event.set("weapon", current_weapon);
    event_manager.emit(shoot_event);
}
```

### ECS Usage

The game uses an Entity-Component-System for game logic. See **[../../../ECS/README.md](../../../ECS/README.md)** for complete ECS documentation.

Quick example:

```cpp
// Create a player entity
entity player = registry.spawn_entity();
registry.emplace_component<position>(player, 100.f, 200.f);
registry.emplace_component<velocity>(player, 0.f, 0.f);
registry.emplace_component<sprite>(player, "player.png");
registry.emplace_component<weapon>(player, WeaponType::LASER);

// Update position based on velocity
auto* positions = registry.get_if<position>();
auto* velocities = registry.get_if<velocity>();
for (auto [pos, vel] : zipper(*positions, *velocities)) {
    pos.x += vel.vx * dt;
    pos.y += vel.vy * dt;
}
```

### Service Layer

Services are singletons that handle cross-cutting concerns:

```cpp
// InputService — Captures keyboard/mouse input
if (input_service.is_key_pressed(KEY_W)) {
    player_velocity.y = -PLAYER_SPEED;
}

// RenderService — Manages rendering
render_service.draw_sprite(texture, x, y, scale);
render_service.draw_text("Score: 1000", 10, 10, 24);

// NetworkService — Sends/receives network messages
network_service.send_position_update(player_x, player_y);
Packet packet = network_service.receive();
```

### Networking Details

For detailed networking architecture and client-side prediction, see **[Networking.md](Networking.md)**.

Key concepts:
- **Client-Side Prediction** — Local inputs feel responsive
- **Server Reconciliation** — Server corrections are smooth
- **Interpolation** — Remote player positions are smooth
- **Reliable Messages** — Critical events always arrive

---

## Adding New Features

### Adding a New Component

Components are data containers for game entities:

```cpp
// File: Entity/Components/Shield.hpp
#pragma once
#include "ECS/Components/IComponent.hpp"

struct shield : public IComponent {
    float health;
    float max_health;
    bool active;
    
    shield(float max_hp = 100.f)
        : health(max_hp), max_health(max_hp), active(true) {}
};
```

Then register it in the ECS library. See **[../../../ECS/README.md](../../../ECS/README.md)** for full instructions.

### Adding a New System

Systems contain game logic:

```cpp
// File: Entity/Systems/ShieldSystem.hpp
#pragma once
#include "ECS/Systems/ISystem.hpp"
#include "../Components/Shield.hpp"

class ShieldSystem : public ISystem {
public:
    void update(registry& reg, float dt) override {
        auto* shields = reg.get_if<shield>();
        auto* sprites = reg.get_if<sprite>();
        
        for (auto [sh, spr] : zipper(*shields, *sprites)) {
            if (sh.active) {
                // Render shield visual
                spr.tint = BLUE;
            }
        }
    }
};
```

### Adding a New State

States manage game flow:

```cpp
// File: Core/States/GameOverState.hpp
#pragma once
#include "IGameState.hpp"

class GameOverState : public IGameState {
private:
    int final_score;
    
public:
    void enter() override {
        // State initialization
        ui_manager.show_game_over_screen(final_score);
    }
    
    void update(float dt) override {
        // Update game over logic
        if (input_service.is_key_pressed(KEY_SPACE)) {
            state_machine.transition_to<MainMenuState>();
        }
    }
    
    void render() override {
        // Draw game over screen
    }
    
    void exit() override {
        // Clean up state
    }
};
```

### Adding a New Event Type

Events enable loose coupling:

```cpp
// In Core/Events.hpp, add:
enum class EventType {
    PLAYER_HEALTH_CHANGED,
    ENEMY_SPAWNED,
    WEAPON_UPGRADED,
    // Add new events here
    GAME_PAUSED,
    LEVEL_COMPLETED,
};

// In your system/state, emit the event:
Event level_complete("LEVEL_COMPLETED");
level_complete.set("level_number", current_level);
level_complete.set("score", player_score);
event_manager.emit(level_complete);

// Subscribe to it elsewhere:
event_manager.subscribe("LEVEL_COMPLETED", [this](const Event& e) {
    int level = e.get<int>("level_number");
    show_level_complete_screen(level);
});
```

### Adding a New Service

Services handle cross-cutting concerns:

```cpp
// File: Core/Services/AudioService.hpp
#pragma once
#include "IService.hpp"

class AudioService : public IService {
public:
    void play_sound(const std::string& sound_name) {
        // Use AudioManager (from ECS)
        AudioManager::instance().get_sfx().play(sound_name);
    }
    
    void set_master_volume(float volume) {
        AudioManager::instance().set_master_volume(volume);
    }
};

// In Application.cpp, add:
AudioService audio_service;
// ... then use it:
audio_service.play_sound("shoot");
```

### Adding UI Elements

UI components display information:

```cpp
// File: UI/Components/EnemyRadar.hpp
#pragma once
#include "ECS/UI/UIComponent.hpp"

class EnemyRadar : public UIComponent {
public:
    void render() override {
        // Draw radar background
        DrawRectangle(10, 10, 100, 100, DARK_GRAY);
        
        // Draw enemy positions
        for (const auto& enemy : nearby_enemies) {
            float radar_x = 10 + (enemy.x / world_width) * 100;
            float radar_y = 10 + (enemy.y / world_height) * 100;
            DrawCircle(radar_x, radar_y, 3, RED);
        }
    }
};
```

---

## Creating New Scenes

Scenes are levels/play areas. Create a new scene by:

1. **Create Scene Configuration** — Define layout, enemies, obstacles
2. **Create Scene Data Component** — Store scene-specific data
3. **Create Scene Loading System** — Populate entities from config
4. **Add Scene to Progression** — Include in level sequence

### Example: Forest Level

```cpp
// File: Entity/Components/SceneData.hpp
struct forest_scene_data {
    int enemy_wave_count;
    float tree_density;
    bool has_boss;
};

// File: Entity/Systems/ForestSceneLoader.hpp
class ForestSceneLoader {
public:
    static void load_scene(registry& reg) {
        // Create terrain
        for (int i = 0; i < 10; i++) {
            create_tree(reg, i * 100, 200);
        }
        
        // Create enemies
        for (int i = 0; i < 5; i++) {
            create_enemy(reg, EnemyType::BASIC, 400 + i * 200, 100);
        }
        
        // Create boss
        create_enemy(reg, EnemyType::BOSS, 500, -100);
    }
};

// In GameState::load_scene():
if (current_scene == Scene::FOREST) {
    ForestSceneLoader::load_scene(registry);
}
```

---

## Creating Plugins/Mods

Plugins extend R-Type with new content. Since R-Type uses a dynamic loading architecture, plugins are compiled as shared libraries and loaded at runtime.

### Plugin System Architecture

R-Type supports two types of plugins:

1. **Component Plugins** — New game data types
2. **System Plugins** — New game logic

Both are compiled as shared libraries (`.so`, `.dll`, `.dylib`) and loaded via the ECS loader.

### Creating a Component Plugin

```cpp
// File: plugins/my_component_plugin/MyComponent.hpp
#pragma once
#include "ECS/Components/IComponent.hpp"

struct my_custom_component : public IComponent {
    float value1;
    int value2;
    std::string data;
    
    my_custom_component() = default;
    my_custom_component(float v1, int v2, const std::string& d)
        : value1(v1), value2(v2), data(d) {}
};
```

```cpp
// File: plugins/my_component_plugin/plugin.cpp
#include "MyComponent.hpp"
#include "ECS/Registry.hpp"

extern "C" {
    // Called when plugin is loaded
    void register_components(registry& reg) {
        reg.register_component<my_custom_component>();
    }
}
```

### Creating a System Plugin

```cpp
// File: plugins/my_system_plugin/MySystem.hpp
#pragma once
#include "ECS/Systems/ISystem.hpp"
#include "MyComponent.hpp"

class MySystem : public ISystem {
public:
    void update(registry& reg, float dt) override {
        auto* components = reg.get_if<my_custom_component>();
        if (!components) return;
        
        for (auto [comp, idx] : zipper(*components)) {
            // Update logic
            comp.value1 += comp.value2 * dt;
        }
    }
};
```

```cpp
// File: plugins/my_system_plugin/plugin.cpp
#include "MySystem.hpp"

extern "C" {
    // Called when plugin is loaded
    ISystem* create_system() {
        return new MySystem();
    }
    
    void destroy_system(ISystem* sys) {
        delete sys;
    }
}
```

### Building a Plugin

```bash
# Create plugin directory
mkdir plugins/my_plugin
cd plugins/my_plugin

# Create CMakeLists.txt
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.10)
project(my_plugin_system)

add_library(my_plugin_system SHARED plugin.cpp)
target_link_libraries(my_plugin_system PRIVATE ECS)
target_include_directories(my_plugin_system PRIVATE ${CMAKE_SOURCE_DIR}/ECS/include)

set_target_properties(my_plugin_system PROPERTIES
    PREFIX "lib"
    SUFFIX ".so"
)
EOF

# Add to main CMakeLists.txt
# add_subdirectory(plugins/my_plugin)

# Build
mkdir build && cd build
cmake ..
cmake --build .
```

### Loading a Plugin

```cpp
// In Application or GameState initialization
#include "ECS/LinuxLoader.hpp"

PlatformLoader loader;
registry reg;

// Load components
loader.load_components("./libECS.so", reg);

// Load system plugins
loader.load_system("./lib/systems/libmy_plugin_system.so", ILoader::LogicSystem);
```

### Plugin Guidelines

- **Keep plugins independent** — Don't create hard dependencies on other plugins
- **Document your plugin** — Include README with component/system descriptions
- **Version your API** — Change major version if breaking changes
- **Handle loading errors** — Return false or throw exception if loading fails
- **Use consistent naming** — Prefix with plugin name (e.g., `my_plugin_component`)

---

## Debugging

### Enable Debug Logging

```cpp
// In main_client.cpp, before app.run():
#ifdef _DEBUG
    EnableDebugLogging();
    SetLogLevel(LogLevel::Verbose);
#endif
```

### Using a Debugger

#### GDB (Linux/macOS)
```bash
gdb ./r-type_client
(gdb) run 127.0.0.1 8080
(gdb) break Application.cpp:42
(gdb) continue
(gdb) print player_position
(gdb) step
```

#### LLDB (macOS)
```bash
lldb ./r-type_client
(lldb) run 127.0.0.1 8080
(lldb) b Application.cpp:42
(lldb) c
(lldb) p player_position
(lldb) s
```

#### Visual Studio (Windows)
1. Set breakpoints in the code
2. Press **F5** to start debugging
3. Use the Debug toolbar to step, continue, watch variables

### Common Issues

**Game runs but networking doesn't work**
- Check server is running
- Verify firewall allows UDP port 8080
- Check event subscriptions are registered before events are emitted

**Frame rate is low**
- Profile with a performance profiler
- Check for expensive O(n²) operations (especially collision detection)
- Use `PhysicsManager` spatial hash instead of naive collision

**Entities not rendering**
- Verify entities have both `position` and `sprite` components
- Check sprite texture paths are correct
- Verify `RenderService` is calling `end_frame()`

---

## Contributing

### Code Style

- Use snake_case for variables and functions
- Use PascalCase for classes and types
- Use UPPER_CASE for constants
- Use 4-space indentation
- Write comments for non-obvious logic

### Workflow

1. Create a feature branch
   ```bash
   git checkout -b feature/my-feature
   ```

2. Make changes and commit
   ```bash
   git add .
   git commit -m "feat: add new feature description"
   ```

3. Update documentation if needed

4. Push and create a pull request
   ```bash
   git push origin feature/my-feature
   ```

### Testing

Before submitting:
```bash
make clean      # Clean build
make            # Full rebuild
make test       # Run tests
./r-type_client 127.0.0.1 8080  # Manual testing
```

---

## Resources

- **[ECS Documentation](../../../ECS/README.md)** — Entity-Component-System API and patterns
- **[Networking Guide](Networking.md)** — Client-side prediction, synchronization, rollback
- **[Protocol Specification](../../../Protocol.md)** — Binary protocol details
- **[Main Project README](../../../README.md)** — Project overview and quick start
- **[UI Documentation](../../../ECS/include/ECS/UI/UIBuilder_DOCUMENTATION.md)** — UI system and UIBuilder API

---

## FAQ

**Q: Can I create my own game using the ECS framework?**  
A: Yes! See **[../README.md](../README.md)** for a game development guide.

**Q: How do I add new weapon types?**  
A: Create a `Weapon` component variant, a `WeaponSystem`, and network messages for the new weapon type.

**Q: Can I modify the protocol?**  
A: Be careful! Changing the protocol requires updates to both client and server. Document all changes in `Protocol.md`.

**Q: How do I optimize collision detection?**  
A: Use `PhysicsManager::instance()` spatial hashing. See **[ECS README](../../../ECS/README.md)** for details.

**Q: Where do I report bugs?**  
A: Open an issue on [GitHub](https://github.com/ColAntoine/R-Type/issues) with a detailed description and reproduction steps.

---

## License

Part of the EPITECH R-Type curriculum project. Educational use only.

---

<div align="center">
  <strong>Built with modern C++17 for high-performance gaming</strong>
  <br>
  <sub>© 2025 R-Type Team - EPITECH</sub>
</div>
