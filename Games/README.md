# Game Development Guide

## Overview

This guide explains how to create a game using the architecture patterns demonstrated in the **R-Type** project. The framework combines a service-oriented architecture, an Entity Component System (ECS), a state machine, and an event-driven design to create scalable, maintainable games.

**Target Audience**: Developers familiar with C++ who want to build games with a clean, modular architecture.

**What You'll Learn**:
- How to structure a game project
- How to use the ECS for game entities
- How to implement game states and UI
- How to handle input, rendering, and networking
- Best practices for game architecture

---

## Architecture Overview

The framework is built on four main pillars:

### 1. **Entity Component System (ECS)**
- **Purpose**: Manages game entities (players, enemies, projectiles)
- **Location**: `../ECS/` library
- **Documentation**: See `../ECS/README.md`
- **Key Concepts**:
  - **Entities**: Unique IDs representing game objects
  - **Components**: Data attached to entities (position, health, sprite)
  - **Systems**: Logic that operates on entities with specific components

### 2. **Service-Oriented Architecture**
- **Purpose**: Manages cross-cutting concerns (input, rendering, networking)
- **Pattern**: Dependency injection with lifecycle management
- **Key Services**:
  - **Input Service**: Captures player input
  - **Render Service**: Handles rendering and window management
  - **Network Service**: Manages client-server communication (optional)

### 3. **State Machine**
- **Purpose**: Manages game flow (menu → gameplay → game over)
- **Pattern**: Stack-based state management
- **Benefits**:
  - Clean separation of game phases
  - Easy state transitions
  - State pause/resume support

### 4. **Event-Driven Communication**
- **Purpose**: Decouples systems through message passing
- **Pattern**: Publisher-subscriber with typed events
- **Benefits**:
  - Reduced coupling between systems
  - Easy to extend with new behaviors
  - Supports immediate and queued processing

---

## Project Structure Template

Here's a recommended folder structure for a new game:

```
YourGame/
├── CMakeLists.txt              # Build configuration
├── main.cpp                    # Entry point
├── Application.hpp/cpp         # Main application orchestrator
├── README.md                   # Game-specific documentation
│
├── Core/                       # Core framework components
│   ├── EventManager.hpp/cpp    # Event system
│   ├── Events.hpp              # Game event definitions
│   │
│   ├── Services/               # Service layer
│   │   ├── ServiceManager.hpp/cpp
│   │   ├── Input/              # Input service
│   │   ├── Render/             # Rendering service
│   │   └── Network/            # Network service (optional)
│   │
│   ├── States/                 # State machine
│   │   ├── GameState.hpp       # State interface
│   │   ├── GameStateManager.hpp/cpp
│   │   ├── MainMenu/           # Menu state
│   │   ├── InGame/             # Gameplay state
│   │   └── GameOver/           # End state
│   │
│   └── Window/                 # Window management (optional)
│       └── GameWindow.hpp/cpp
│
├── Entity/                     # Game-specific ECS extensions
│   ├── Components/             # Custom components
│   │   ├── Player/
│   │   ├── Enemy/
│   │   └── ...
│   │
│   └── Systems/                # Custom systems
│       ├── Movement/
│       ├── Combat/
│       └── ...
│
└── UI/                         # User interface
    ├── UIManager.hpp/cpp       # UI coordinator
    ├── UIComponent.hpp         # UI base interface
    └── Components/             # UI widgets
        ├── UIButton.hpp/cpp
        ├── UIText.hpp/cpp
        └── ...
```

---

## Step-by-Step: Creating a Basic Game

### Step 1: Set Up Your Project

**1.1 Create Project Structure**
```bash
mkdir MyGame && cd MyGame
mkdir -p Core/Services/{Input,Render} Core/States/{MainMenu,InGame,GameOver}
mkdir -p Entity/{Components,Systems}
mkdir -p UI/Components
```

**1.2 Create CMakeLists.txt**
```cmake
cmake_minimum_required(VERSION 3.10)
project(MyGame VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include ECS library
include_directories(../ECS/include)
link_directories(../ECS/build)

# Find dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(RAYLIB REQUIRED raylib)

# Source files
file(GLOB_RECURSE SOURCES "*.cpp")

# Executable
add_executable(my_game ${SOURCES})
target_link_libraries(my_game ECS ${RAYLIB_LIBRARIES} dl)
```

---

### Step 2: Implement Core Framework

**2.1 Create Event System (`Core/EventManager.hpp/cpp`)**

See `RType/Core/EventManager.hpp` for reference implementation.

Key features:
- Template-based type-safe event subscription
- Support for immediate and queued event processing
- Multiple handlers per event type

**2.2 Define Your Events (`Core/Events.hpp`)**
```cpp
#pragma once
#include "EventManager.hpp"

// Input Events
struct KeyPressEvent : Event {
    int key;
    KeyPressEvent(int k) : key(k) {}
};

// Game Events
struct PlayerSpawnEvent : Event {
    int player_id;
    float x, y;
    PlayerSpawnEvent(int id, float x_, float y_) 
        : player_id(id), x(x_), y(y_) {}
};

struct GameOverEvent : Event {
    int final_score;
    GameOverEvent(int score) : final_score(score) {}
};

// Add more events as needed...
```

**2.3 Create Service Manager (`Core/Services/ServiceManager.hpp/cpp`)**

See `RType/Core/Services/ServiceManager.hpp` for reference.

Key features:
- Service registration with dependency injection
- Type-safe service retrieval
- Automatic lifecycle management (initialize, update, shutdown)

**2.4 Implement Basic Services**

**Input Service** (`Core/Services/Input/Input.hpp/cpp`):
- Captures keyboard/mouse input
- Emits input events
- See `RType/Core/Services/Input/` for reference

**Render Service** (`Core/Services/Render/Render.hpp/cpp`):
- Window management
- Frame rendering
- Camera control
- See `RType/Core/Services/Render/` for reference

---

### Step 3: Implement State Machine

**3.1 Create State Interface (`Core/States/GameState.hpp`)**
```cpp
#pragma once

class IGameState {
public:
    virtual ~IGameState() = default;
    
    virtual void on_enter() = 0;    // Called when state becomes active
    virtual void on_exit() = 0;     // Called when state is removed
    virtual void on_pause() = 0;    // Called when another state is pushed
    virtual void on_resume() = 0;   // Called when top state is popped
    
    virtual void update(float dt) = 0;  // Called every frame
    virtual void render() = 0;          // Called for rendering
};
```

**3.2 Create State Manager (`Core/States/GameStateManager.hpp/cpp`)**

See `RType/Core/States/GameStateManager.hpp` for reference implementation.

Key features:
- Stack-based state management
- Safe state transitions (push, pop, change)
- State factory pattern for easy registration

**3.3 Implement Game States**

**MainMenu State** (`Core/States/MainMenu/MainMenu.hpp/cpp`):
```cpp
class MainMenu : public IGameState {
private:
    UIManager ui_manager_;
    ServiceManager& services_;
    GameStateManager& _stateManager;

public:
    MainMenu(ServiceManager& services, GameStateManager& states)
        : services_(services), _stateManager(states) {}
    
    void on_enter() override {
        // Create UI buttons
        auto play_btn = std::make_shared<UIButton>(
            "Play", 300, 200, 200, 50
        );
        play_btn->set_callback([this]() {
            _stateManager.change_state("InGame");
        });
        ui_manager_.add_component("play_button", play_btn);
    }
    
    void on_exit() override {
        ui_manager_.clear_components();
    }
    
    void update(float dt) override {
        ui_manager_.update(dt);
    }
    
    void render() override {
        ui_manager_.render();
    }
    
    void on_pause() override {}
    void on_resume() override {}
};
```

**InGame State** (`Core/States/InGame/InGame.hpp/cpp`):
```cpp
class InGame : public IGameState {
private:
    registry& ecs_registry_;
    ServiceManager& services_;
    EventManager& events_;
    entity player_entity_;

public:
    InGame(registry& ecs, ServiceManager& services, EventManager& events)
        : ecs_registry_(ecs), services_(services), events_(events) {}
    
    void on_enter() override {
        // Create player entity
        player_entity_ = ecs_registry_.spawn_entity();
        ecs_registry_.emplace_component<position>(player_entity_, 400, 300);
        ecs_registry_.emplace_component<velocity>(player_entity_, 0, 0);
        ecs_registry_.emplace_component<sprite>(
            player_entity_, "assets/player.png", 64, 64
        );
        
        // Subscribe to game events
        events_.subscribe<GameOverEvent>([this](const GameOverEvent& e) {
            // Handle game over
        });
    }
    
    void on_exit() override {
        // Cleanup entities
        ecs_registry_.kill_entity(player_entity_);
    }
    
    void update(float dt) override {
        // Update ECS systems
        // (systems handle movement, collision, etc.)
    }
    
    void render() override {
        // Systems handle rendering
    }
    
    void on_pause() override {}
    void on_resume() override {}
};
```

---

### Step 4: Create Game-Specific Components and Systems

**4.1 Define Custom Components (`Entity/Components/`)**

Example: Player Component
```cpp
// Entity/Components/Player/Player.hpp
#pragma once
#include "ECS/Components/IComponent.hpp"

struct player : public IComponent {
    int lives{3};
    int score{0};
    float speed{200.0f};
    
    player() = default;
    player(int l, int s, float sp) : lives(l), score(s), speed(sp) {}
};
```

Example: Enemy Component
```cpp
// Entity/Components/Enemy/Enemy.hpp
#pragma once
#include "ECS/Components/IComponent.hpp"

struct enemy : public IComponent {
    int enemy_type{0};
    float health{100.0f};
    float damage{10.0f};
    
    enemy() = default;
    enemy(int type, float hp, float dmg) 
        : enemy_type(type), health(hp), damage(dmg) {}
};
```

**4.2 Implement Custom Systems (`Entity/Systems/`)**

Example: Player Control System
```cpp
// Entity/Systems/PlayerControl/PlayerControl.hpp
#pragma once
#include "ECS/Systems/ISystem.hpp"

class PlayerControlSystem : public ISystem {
public:
    void update(registry& reg, float dt) override {
        auto& positions = reg.get<position>();
        auto& velocities = reg.get<velocity>();
        auto& players = reg.get<player>();
        
        // Iterate over player entities
        for (size_t i = 0; i < positions.size(); ++i) {
            if (!players[i].has_value()) continue;
            
            auto& pos = positions[i].value();
            auto& vel = velocities[i].value();
            auto& plr = players[i].value();
            
            // Handle input
            vel.vx = 0;
            vel.vy = 0;
            
            if (IsKeyDown(KEY_LEFT))  vel.vx = -plr.speed;
            if (IsKeyDown(KEY_RIGHT)) vel.vx = plr.speed;
            if (IsKeyDown(KEY_UP))    vel.vy = -plr.speed;
            if (IsKeyDown(KEY_DOWN))  vel.vy = plr.speed;
            
            // Update position
            pos.x += vel.vx * dt;
            pos.y += vel.vy * dt;
        }
    }
    
    const char* get_name() const override { 
        return "PlayerControlSystem"; 
    }
};
```

---

### Step 5: Create Application Class

**5.1 Main Application (`Application.hpp/cpp`)**

See `RType/Application.hpp` for full reference.

Key responsibilities:
```cpp
class Application {
private:
    EventManager event_manager_;
    ServiceManager service_manager_;
    GameStateManager _stateManager;
    registry ecs_registry_;
    bool running_{false};

public:
    bool initialize() {
        // 1. Load ECS library
        ILoader loader;
        loader.load_components("../ECS/build/libECS.so", ecs_registry_);
        
        // 2. Register services
        service_manager_.register_service<InputService>(event_manager_);
        service_manager_.register_service<RenderService>();
        service_manager_.initialize_all();
        
        // 3. Register game states
        _stateManager.register_state<MainMenu>("MainMenu");
        _stateManager.register_state<InGame>("InGame");
        _stateManager.register_state<GameOver>("GameOver");
        
        // 4. Start with main menu
        _stateManager.push_state("MainMenu");
        
        running_ = true;
        return true;
    }
    
    void run() {
        while (running_ && !WindowShouldClose()) {
            float dt = GetFrameTime();
            
            // 1. Update services
            service_manager_.update_all(dt);
            
            // 2. Process events
            event_manager_.process_queue();
            
            // 3. Update active state
            _stateManager.update(dt);
            
            // 4. Update ECS systems
            // (called by active state or here)
            
            // 5. Render
            BeginDrawing();
            ClearBackground(BLACK);
            _stateManager.render();
            EndDrawing();
        }
    }
    
    void shutdown() {
        _stateManager.clear();
        service_manager_.shutdown_all();
        running_ = false;
    }
};
```

**5.2 Main Entry Point (`main.cpp`)**
```cpp
#include "Application.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "=== My Game ===" << std::endl;
    
    Application app;
    
    try {
        if (!app.initialize()) {
            std::cerr << "Failed to initialize" << std::endl;
            return 1;
        }
        
        app.run();
        app.shutdown();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

---

## UI System

### Creating UI Components

The UI system uses a component-based architecture similar to the ECS.

**Base UI Interface** (`UI/UIComponent.hpp`):
```cpp
class IUIComponent {
public:
    virtual ~IUIComponent() = default;
    
    virtual void update(float dt) = 0;
    virtual void render() = 0;
    virtual bool handle_input() = 0;  // Returns true if input consumed
    virtual bool is_mouse_over(float x, float y) = 0;
    
    virtual void set_visible(bool visible) = 0;
    virtual void set_enabled(bool enabled) = 0;
    virtual bool is_visible() const = 0;
    virtual bool is_enabled() const = 0;
};
```

**Example: Button Component** (`UI/Components/UIButton.hpp/cpp`):
```cpp
class UIButton : public IUIComponent {
private:
    std::string text_;
    Rectangle bounds_;
    std::function<void()> callback_;
    bool hovered_{false};
    bool pressed_{false};
    bool visible_{true};
    bool enabled_{true};

public:
    UIButton(const std::string& text, float x, float y, float w, float h)
        : text_(text), bounds_{x, y, w, h} {}
    
    void set_callback(std::function<void()> cb) { callback_ = cb; }
    
    void update(float dt) override {
        if (!visible_ || !enabled_) return;
        
        Vector2 mouse = GetMousePosition();
        hovered_ = CheckCollisionPointRec(mouse, bounds_);
    }
    
    void render() override {
        if (!visible_) return;
        
        Color bg_color = enabled_ 
            ? (hovered_ ? DARKGRAY : GRAY)
            : LIGHTGRAY;
        
        DrawRectangleRec(bounds_, bg_color);
        DrawRectangleLinesEx(bounds_, 2, BLACK);
        
        int text_width = MeasureText(text_.c_str(), 20);
        DrawText(text_.c_str(),
                 bounds_.x + (bounds_.width - text_width) / 2,
                 bounds_.y + (bounds_.height - 20) / 2,
                 20, BLACK);
    }
    
    bool handle_input() override {
        if (!visible_ || !enabled_ || !hovered_) return false;
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (callback_) callback_();
            return true;  // Input consumed
        }
        return false;
    }
    
    bool is_mouse_over(float x, float y) override {
        return CheckCollisionPointRec({x, y}, bounds_);
    }
    
    // Getters/setters...
};
```

**UI Manager** (`UI/UIManager.hpp/cpp`):

See `RType/UI/UIManager.hpp` for reference implementation.

---

## Best Practices

### 1. **Separation of Concerns**
- **ECS**: Entity data and behavior
- **Services**: Cross-cutting concerns (input, rendering, networking)
- **States**: Game flow and lifecycle
- **Events**: Communication between systems

### 2. **Component Design**
- Keep components as plain data structures (POD)
- No logic in components (logic goes in systems)
- Small, focused components (single responsibility)

### 3. **System Design**
- Systems should be stateless when possible
- One system per behavior (movement, collision, rendering)
- Systems communicate through events, not direct calls

### 4. **Event Usage**
- Use events for asynchronous communication
- Keep event payloads small
- Use immediate mode for time-critical events
- Use queued mode for batch processing

### 5. **State Management**
- Keep states focused on a single game phase
- Clean up resources in `on_exit()`
- Use `on_pause()`/`on_resume()` for overlays (pause menu, etc.)

### 6. **Performance**
- Minimize component additions/removals during gameplay
- Use object pools for frequently created/destroyed entities
- Profile before optimizing
- Cache component references when iterating

### 7. **Project Organization**
```
Guideline: Group by feature, not by type
Good:  Entity/Components/Player/, Entity/Systems/PlayerControl/
Bad:   Components/, Systems/ (all mixed together)
```

---

## Common Patterns

### Pattern 1: Player Input → Entity Movement
```
Input Service (captures keys)
  ↓ emit KeyPressEvent
Control System (receives event)
  ↓ updates velocity component
Position System (reads velocity)
  ↓ updates position component
Render System (reads position + sprite)
  ↓ draws to screen
```

### Pattern 2: Enemy Spawning
```
Spawn System (timer-based)
  ↓ emit EnemySpawnEvent
Enemy System (receives event)
  ↓ creates entity with enemy components
  ↓ sets position, sprite, health, etc.
```

### Pattern 3: Collision Detection
```
Collision System
  ↓ iterates entities with collider component
  ↓ checks AABB collision
  ↓ emit CollisionEvent(entity_a, entity_b)
Damage System (receives event)
  ↓ reduces health component
  ↓ checks if health <= 0
  ↓ emit EntityDestroyEvent
```

### Pattern 4: State Transitions
```
InGame State
  ↓ player health reaches 0
  ↓ emit GameOverEvent
GameStateManager (receives event)
  ↓ change_state("GameOver")
GameOver State
  ↓ on_enter() displays score
  ↓ waits for input
  ↓ transitions to MainMenu
```

---

## Example: Simple Shooter Game

Here's a minimal complete example combining everything:

### Game Concept
- Player controlled ship (arrow keys to move, space to shoot)
- Enemies spawn from top of screen
- Collision detection (bullets hit enemies)
- Score tracking

### Required Components
```cpp
// Player ship
struct player { float speed; int score; };

// Enemy
struct enemy { int type; float health; };

// Projectile
struct projectile { float damage; int owner_id; };

// From ECS library: position, velocity, sprite, collider
```

### Required Systems
```cpp
// 1. PlayerControlSystem: handles input → velocity
// 2. EnemySpawnSystem: spawns enemies periodically
// 3. ProjectileSystem: moves bullets, checks lifetime
// 4. CollisionSystem: detects collisions → emits events
// 5. HealthSystem: processes damage
// 6. RenderSystem: draws all sprites
```

### Game Flow
```
MainMenu
  ↓ user clicks "Play"
InGame
  ↓ player destroyed
GameOver
  ↓ user clicks "Retry"
MainMenu
```

### Build and Run
```bash
cd MyGame
mkdir build && cd build
cmake ..
make
./my_game
```

---

## Advanced Topics

### Networking (Optional)
If you want multiplayer, add:
- **Network Service**: UDP client/server communication
- **Network System**: Serializes/deserializes game state
- **NetworkSync Component**: Marks entities that sync over network

See `RType/Core/Services/Network/` and `../Network/Protocol.md` for reference.

### Asset Management
For larger games, consider:
- Asset manager service for loading/caching textures, sounds
- Resource pools to avoid repeated loading
- Async loading with loading screens

### Save System
Implement save/load:
- Serialize ECS state to file
- Store player progress, settings
- Use events for save/load triggers

### Audio System
Add audio service:
- Sound effects (explosions, shooting)
- Background music
- Volume control

---

## Debugging Tips

### Common Issues

**1. Entities not rendering**
- Check if sprite component has valid texture path
- Verify RenderSystem is updating
- Ensure entity has both position and sprite components

**2. Input not working**
- Check if InputService is initialized and updating
- Verify event subscriptions are set up
- Check if UI is consuming input (buttons, etc.)

**3. State transitions not working**
- Ensure states are registered in StateManager
- Check if state names match exactly
- Verify `on_exit()` is cleaning up properly

**4. Systems not updating entities**
- Check component types match exactly
- Verify entities have required components
- Use zipper utility for multi-component iteration

### Debug Tools

**Enable Logging**:
```cpp
// In EventManager
void emit(const T& event) {
    std::cout << "Event emitted: " << typeid(T).name() << std::endl;
    // ...
}
```

**Entity Inspector**:
```cpp
void debug_print_entities(registry& reg) {
    std::cout << "Total entities: " << reg.entity_count() << std::endl;
    // Print component counts...
}
```

**Frame Time Monitor**:
```cpp
void Application::run() {
    while (running_) {
        float dt = GetFrameTime();
        std::cout << "FPS: " << (1.0f / dt) << std::endl;
        // ...
    }
}
```

---

## Reference Projects

### Minimal Game (Recommended Starting Point)
**SimpleShooter** - Single player, no networking
```
- 1 state (InGame)
- 3 components (player, enemy, projectile)
- 4 systems (control, spawn, collision, render)
- ~500 lines of code
```

### Medium Game
**R-Type Clone** - Multiplayer, full featured
```
- Multiple states (Menu, Lobby, InGame, GameOver)
- 10+ components
- 10+ systems
- UI system
- Network integration
- ~3000 lines of code
```

See `RType/` folder for full implementation.

---

## Additional Resources

- **ECS Documentation**: `../ECS/README.md`
- **R-Type Client**: `RType/README.md` (full example)
- **Network Protocol**: `../Network/Protocol.md` (if using networking)
- **Raylib Documentation**: https://www.raylib.com/

---

## Contributing

When creating games with this framework:

1. **Share your components/systems**: If you create useful generic components (e.g., pathfinding, particle effects), consider making them reusable.

2. **Report issues**: If you find bugs in the ECS or framework code, please report them.

3. **Improve documentation**: Found something unclear? Submit improvements.

---

## License

This framework is part of the R-Type project at EPITECH.

---

## Quick Start Checklist

- [ ] Copy project structure template
- [ ] Set up CMakeLists.txt
- [ ] Implement EventManager and Events
- [ ] Create ServiceManager and basic services (Input, Render)
- [ ] Implement StateManager and game states
- [ ] Define game-specific components
- [ ] Implement game-specific systems
- [ ] Create Application class
- [ ] Implement UI (optional)
- [ ] Build and test
- [ ] Add gameplay features iteratively

---

<div align="center">
  <strong>Build amazing games with clean architecture</strong>
  <br>
  <sub>© 2025 R-Type Team - EPITECH</sub>
</div>
