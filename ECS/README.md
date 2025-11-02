# R-Type ECS Engine Documentation

## Overview
A high-performance **Entity Component System (ECS)** engine built in C++17 for the R-Type game project. Features dynamic plugin loading, advanced rendering, physics optimization, and comprehensive asset management.

### What is ECS?
**Entities** are unique IDs representing game objects.  
**Components** are pure data (position, velocity, sprite, etc.).  
**Systems** contain logic that operates on entities with specific components.

This separation enables:
- **Modularity**: Easy addition/removal of features
- **Performance**: Cache-friendly data layout
- **Flexibility**: Runtime component/system loading

---

## Core Architecture

### Registry (`include/ECS/Registry.hpp`)
The heart of the ECS. Manages entities and components using type-safe sparse sets.

**Key Features:**
- **Sparse Set Storage**: O(1) component access, cache-friendly iteration
- **Type-Safe**: Template-based component registration
- **Entity Management**: Spawn, kill, and query entities efficiently

**API:**
```cpp
registry reg;

// Spawn entity
entity player = reg.spawn_entity();

// Add components
reg.emplace_component<position>(player, 100.f, 200.f);
reg.add_component<velocity>(player, velocity{5.f, 0.f});

// Query components
auto* positions = reg.get_if<position>();
auto& player_pos = reg.get_component<position>(player);

// Remove component
reg.remove_component<velocity>(player);

// Kill entity (auto-removes all components)
reg.kill_entity(player);
```

### Components (`include/ECS/Components/`)
Pure data structures. **Core components included:**

| Component | Purpose | Fields |
|-----------|---------|--------|
| `position` | 2D coordinates | `x`, `y` |
| `velocity` | Movement speed | `vx`, `vy` |
| `sprite` | Static texture | `texture_path`, `width`, `height`, `scale`, `rotation` |
| `animation` | Animated sprite | `texture_path`, `frame_w/h`, `frame_count`, `fps`, `loop` |
| `collider` | Collision box | `w`, `h`, `offset_x/y`, `is_trigger` |

**Custom components** can be added in `Games/RType/Entity/Components/`.

### Systems (`include/ECS/Systems/`)
Logic that operates on components. **Core systems:**

| System | Function | Components Used |
|--------|----------|----------------|
| `position_system` | Move entities | `position`, `velocity` |
| `collision_system` | Detect collisions | `position`, `collider` |
| `sprite_system` | Render sprites | `position`, `sprite` |
| `animation_system` | Animate sprites | `position`, `animation` |

### Dynamic Loading (`include/ECS/ILoader.hpp`)
Load components and systems at runtime without recompilation.

**Interface:**
```cpp
class ILoader {
    public:
        // Load component library (libECS.so)
        virtual bool load_components(const std::string& path, registry& reg) = 0;
        virtual IComponentFactory* get_factory() const = 0;

        // Load system library (libcollision_system.so, etc.)
        virtual bool load_system(const std::string& path, SystemType type) = 0;
        virtual void update_all_systems(registry& r, float dt, SystemType type) = 0;

        // SystemType: LogicSystem (gameplay) or RenderSystem (drawing)
        enum SystemType { LogicSystem, RenderSystem };
};
```

**Usage:**
```cpp
#include "ECS/LinuxLoader.hpp"  // or WinLoader.hpp

PlatformLoader loader;
registry reg;

// Load components
loader.load_components("./libECS.so", reg);

// Load systems
loader.load_system("./lib/systems/libcollision_system.so", ILoader::LogicSystem);
loader.load_system("./lib/systems/libsprite_system.so", ILoader::RenderSystem);

// Game loop
while (running) {
    loader.update_all_systems(reg, dt, ILoader::LogicSystem);  // Physics, gameplay
    loader.update_all_systems(reg, dt, ILoader::RenderSystem); // Rendering
}
```

### Zipper Utility (`include/ECS/Zipper.hpp`)
Iterate over multiple components simultaneously.

**Example:**
```cpp
// Update positions based on velocity
auto* positions = reg.get_if<position>();
auto* velocities = reg.get_if<velocity>();

for (auto [pos, vel] : zipper(*positions, *velocities)) {
    pos.x += vel.vx * dt;
    pos.y += vel.vy * dt;
}
```

Only iterates entities that have **all** specified components.

---

## Engine Managers (Singletons)

### AssetManager (`include/ECS/AssetManager/`)
Caches textures and sounds to prevent duplicate loading.

```cpp
auto& assets = AssetManager::instance();
assets.init();

Texture2D* tex = assets.get_texture("player.png");  // Cached!
Sound* sfx = assets.get_sound("shoot.wav");         // Cached!

assets.unload_texture("player.png");  // Manual cleanup
```

**Benefits:** Prevents memory waste, automatic resource management.

### RenderManager (`include/ECS/Renderer/`)
Handles window, camera, and sprite batching for optimized rendering.

```cpp
auto& renderer = RenderManager::instance();
renderer.init("R-Type");

renderer.begin_frame();
    auto& batch = renderer.get_batch();
    batch.draw(texture, position, WHITE, layer);  // Batched!
renderer.end_frame();

// Camera control
auto& camera = renderer.get_camera();
camera.set_position(player_x, player_y);
camera.set_zoom(2.0f);
```

**Benefits:** Sprite batching (fewer draw calls), Z-ordering, camera system.

### PhysicsManager (`include/ECS/Physics/`)
Optimizes collision detection using spatial hashing (O(n²) → O(n)).

```cpp
auto& physics = PhysicsManager::instance();
physics.init(64.0f);  // Grid cell size

// Update entities in spatial grid
for (auto [pos, col, entity] : entities) {
    AABB bounds(pos.x, pos.y, col.w, col.h);
    physics.update_entity(entity, bounds);
}

// Get only nearby collision pairs (O(n) instead of O(n²)!)
auto pairs = physics.get_collision_pairs();
for (auto [e1, e2] : pairs) {
    if (physics.check_collision(bounds1, bounds2)) {
        // Handle collision
    }
}
```

**Benefits:** 100x faster collision detection with many entities.

### AudioManager (`include/ECS/Audio/`)
Controls music streaming, sound effects, and 3D positional audio.

```cpp
auto& audio = AudioManager::instance();
audio.init();

// Music
auto& music = audio.get_music();
music.load("menu", "music/menu.ogg");
music.play("menu", true);  // Loop
music.fade_in(2.0f);       // Fade in over 2 seconds

// Sound effects
auto& sfx = audio.get_sfx();
sfx.load("shoot", "sfx/shoot.wav");
sfx.play("shoot", 0.8f);  // Volume 0.8

// 3D positional audio
sfx.set_listener_position({player_x, player_y});
sfx.play_at("explosion", {enemy_x, enemy_y}, 200.0f);  // Max distance

// Master controls
audio.set_master_volume(0.5f);
audio.mute();
```

**Benefits:** Music streaming, fade in/out, 3D audio, master volume control.

### MessagingManager (`include/ECS/Messaging/`)
Event-driven architecture for decoupled system communication.

**EVENTS** (something happened - async broadcast):
```cpp
auto& bus = MessagingManager::instance().get_event_bus();

// Subscribe
bus.subscribe("COLLISION_ENTER", [](const Event& e) {
    size_t entity_a = e.get<size_t>("entity_a");
    size_t entity_b = e.get<size_t>("entity_b");
    // Play sound, show particles, etc.
});

// Emit (all subscribers notified)
Event collision("COLLISION_ENTER");
collision.set("entity_a", (size_t)42);
collision.set("entity_b", (size_t)99);
bus.emit(collision);
```

**COMMANDS** (something must be done - sync request/reply):
```cpp
auto& dispatcher = MessagingManager::instance().get_command_dispatcher();

// Register handler
dispatcher.register_handler("get_entity_info", [](Command& cmd) {
    size_t entity = cmd.get_param<size_t>("entity_id");
    // Do work...
    cmd.set_reply("position_x", 100.0f);
    cmd.set_reply("position_y", 200.0f);
});

// Send command (waits for reply)
Command cmd(CommandType::CUSTOM, "get_entity_info");
cmd.set_param("entity_id", (size_t)42);
dispatcher.dispatch(cmd);
float x = cmd.get_reply<float>("position_x");
```

**Benefits:** Decoupled systems, event-driven, type-safe with `std::any`.

---

## Building the ECS

### Build Instructions
```bash
cd ECS
./build.sh

# Outputs:
# - build/lib/libECS.so (component library)
# - build/lib/systems/*.so (system libraries)
```

### CMake Integration
```cmake
# In your game's CMakeLists.txt
target_link_libraries(r-type_client PRIVATE libECS)
target_link_libraries(r-type_client PRIVATE collision_system)
target_link_libraries(r-type_client PRIVATE sprite_system)
```

---

## Code Examples

### Creating Entities with Components
```cpp
// Simple moving entity
auto player = reg.spawn_entity();
reg.emplace_component<position>(player, 100.f, 200.f);
reg.emplace_component<velocity>(player, 5.f, 0.f);
reg.emplace_component<animation>(player, "player.png", 64.f, 64.f, 0.5f, 0.5f, 8, true);
reg.emplace_component<collider>(player, 64.f, 64.f, -32.f, -32.f);
```

### Writing a Simple System
```cpp
// Position system: Move entities based on velocity
class PositionSystem : public ISystem {
public:
    void update(registry& reg, float dt) override {
        auto* positions = reg.get_if<position>();
        auto* velocities = reg.get_if<velocity>();
        
        if (!positions || !velocities) return;
        
        for (auto [pos, vel] : zipper(*positions, *velocities)) {
            pos.x += vel.vx * dt;
            pos.y += vel.vy * dt;
        }
    }
};
```

### Using the Component Factory
```cpp
auto factory = loader.get_factory();

// Create components dynamically
factory->create_component<position>(reg, entity, 100.f, 200.f);
factory->create_component<velocity>(reg, entity, 5.f, 0.f);
factory->create_component<collider>(reg, entity, 64.f, 64.f, -32.f, -32.f);
```

### Optimized Collision Detection with PhysicsManager
```cpp
// Old way: O(n²) - Check every entity against every other
for (auto e1 : entities) {
    for (auto e2 : entities) {
        check_collision(e1, e2);  // 10,000 checks for 100 entities!
    }
}

// New way: O(n) - Use spatial hash
auto& physics = PhysicsManager::instance();
physics.clear();

// Insert all entities
for (auto [pos, col, ent] : zipper(*positions, *colliders)) {
    AABB bounds(pos.x, pos.y, col.w, col.h);
    physics.update_entity(static_cast<size_t>(ent), bounds);
}

// Get only nearby pairs
auto pairs = physics.get_collision_pairs();  // ~50 checks for 100 entities!
for (auto [e1, e2] : pairs) {
    // Actual collision check only on nearby entities
}
```

### Event-Driven Communication
```cpp
// System A: Emit event when entity dies
Event death_event("ENTITY_DIED");
death_event.set("entity_id", (size_t)42);
death_event.set("score_value", 100);
MessagingManager::instance().get_event_bus().emit(death_event);

// System B: Listen for death events
auto& bus = MessagingManager::instance().get_event_bus();
bus.subscribe("ENTITY_DIED", [](const Event& e) {
    size_t entity = e.get<size_t>("entity_id");
    int score = e.get<int>("score_value");
    // Play death animation, add score, etc.
});
```

---

## Performance Characteristics

| Operation | Time Complexity | Notes |
|-----------|----------------|-------|
| Spawn entity | O(1) | Constant time |
| Add component | O(1) | Sparse set insertion |
| Get component | O(1) | Direct array access |
| Remove component | O(1) | Swap-and-pop |
| Kill entity | O(C) | C = component types on entity |
| Iterate components | O(N) | N = entities with component |
| Collision (naive) | O(n²) | Check all pairs |
| Collision (spatial hash) | O(n) | Only check nearby entities |

**Spatial Hash Performance:**
- 100 entities: 10,000 checks → ~50 checks (200x faster)
- 1000 entities: 1,000,000 checks → ~500 checks (2000x faster)

---

## Architecture Summary

### Design Patterns Used
- **Entity-Component-System**: Data-oriented game architecture
- **Singleton**: All manager classes (AssetManager, RenderManager, etc.)
- **Factory**: IComponentFactory for dynamic component creation
- **Plugin Architecture**: Dynamic system loading via shared libraries
- **Publish-Subscribe**: EventBus for event-driven communication
- **Request-Reply**: CommandDispatcher for synchronous queries
- **Spatial Hashing**: PhysicsManager for collision optimization
- **Object Pool**: SpriteBatch for render batching

### Directory Structure
```
ECS/
├── include/ECS/
│   ├── Registry.hpp           # Core ECS registry
│   ├── Entity.hpp             # Entity type definition
│   ├── ILoader.hpp            # Dynamic loading interface
│   ├── ComponentFactory.hpp   # Component factory interface
│   ├── Zipper.hpp             # Multi-component iteration
│   ├── Components/            # Core components
│   │   ├── Position.hpp
│   │   ├── Velocity.hpp
│   │   ├── Sprite.hpp
│   │   ├── Animation.hpp
│   │   └── Collider.hpp
│   ├── Systems/               # System interfaces
│   │   ├── ISystem.hpp
│   │   ├── Position.hpp
│   │   ├── Collision.hpp
│   │   ├── Sprite.hpp
│   │   └── Animation.hpp
│   ├── AssetManager/          # Resource caching
│   ├── Renderer/              # Rendering pipeline
│   ├── Physics/               # Collision optimization
│   ├── Audio/                 # Sound system
│   ├── Messaging/             # Event/command system
│   └── UI/                    # UI components
└── src/                       # Implementation files
```

---

## For Developers: Extending the ECS

### Adding a New Component
1. **Create header** in `include/ECS/Components/YourComponent.hpp`:
```cpp
#pragma once
#include "ECS/Components/IComponent.hpp"

struct your_component : public IComponent {
    float data1;
    int data2;
    
    your_component();
    your_component(float d1, int d2);
};
```

2. **Implement** in `src/components/your_component.cpp`:
```cpp
#include "ECS/Components/YourComponent.hpp"

your_component::your_component() : data1(0.f), data2(0) {}
your_component::your_component(float d1, int d2) : data1(d1), data2(d2) {}
```

3. **Register** in `src/components/library.cpp`:
```cpp
void register_components(registry &r) {
    r.register_component<position>();
    r.register_component<velocity>();
    // ... existing components ...
    r.register_component<your_component>();  // Add this
}
```

4. **Rebuild**: `cd ECS && ./build.sh`

### Adding a New System
1. **Create system** in `src/systems/your_system/`:
```cpp
// your_system.cpp
#include "ECS/Systems/ISystem.hpp"
#include "ECS/Components/YourComponent.hpp"

class YourSystem : public ISystem {
public:
    void update(registry& reg, float dt) override {
        auto* comps = reg.get_if<your_component>();
        if (!comps) return;
        
        for (auto [comp, idx] : zipper(*comps)) {
            // Your logic here
        }
    }
};

extern "C" {
    ISystem* create_system() { return new YourSystem(); }
    void destroy_system(ISystem* sys) { delete sys; }
}
```

2. **Add to CMake** in `ECS/CMakeLists.txt`:
```cmake
add_library(your_system SHARED src/systems/your_system/your_system.cpp)
target_link_libraries(your_system PRIVATE ECS)
```

3. **Load in game**:
```cpp
loader.load_system("./lib/systems/libyour_system.so", ILoader::LogicSystem);
```

---

## For Users: Quick Start Guide

### Step 1: Initialize the ECS
```cpp
#include "ECS/Registry.hpp"
#include "ECS/LinuxLoader.hpp"

int main() {
    registry reg;
    PlatformLoader loader;
    
    // Load components
    loader.load_components("./libECS.so", reg);
    
    // Load systems
    loader.load_system("./lib/systems/libposition_system.so", ILoader::LogicSystem);
    loader.load_system("./lib/systems/libcollision_system.so", ILoader::LogicSystem);
    loader.load_system("./lib/systems/libsprite_system.so", ILoader::RenderSystem);
    
    return 0;
}
```

### Step 2: Initialize Managers
```cpp
// Asset loading
AssetManager::instance().init();

// Rendering
RenderManager::instance().init("My Game");

// Physics
PhysicsManager::instance().init(64.0f);

// Audio
AudioManager::instance().init();

// Messaging
MessagingManager::instance().init();
```

### Step 3: Create Entities
```cpp
auto factory = loader.get_factory();

entity player = reg.spawn_entity();
factory->create_component<position>(reg, player, 100.f, 200.f);
factory->create_component<velocity>(reg, player, 0.f, 0.f);
factory->create_component<animation>(reg, player, "player.png", 64.f, 64.f, 1.f, 1.f, 8, true);
factory->create_component<collider>(reg, player, 64.f, 64.f, -32.f, -32.f);
```

### Step 4: Game Loop
```cpp
while (!RenderManager::instance().should_close()) {
    float dt = GetFrameTime();
    
    // Update logic systems
    loader.update_all_systems(reg, dt, ILoader::LogicSystem);
    
    // Render
    auto& renderer = RenderManager::instance();
    renderer.begin_frame();
        loader.update_all_systems(reg, dt, ILoader::RenderSystem);
    renderer.end_frame();
}
```

### Step 5: Cleanup
```cpp
AssetManager::instance().shutdown();
RenderManager::instance().shutdown();
AudioManager::instance().shutdown();
```

---

## Troubleshooting

**Q: Components not registered?**  
A: Call `loader.load_components()` before creating entities.

**Q: Systems not updating?**  
A: Check that systems are loaded with correct `SystemType` (LogicSystem vs RenderSystem).

**Q: Collision detection slow?**  
A: Use `PhysicsManager` spatial hash instead of O(n²) checks.

**Q: Textures loading multiple times?**  
A: Use `AssetManager::instance().get_texture()` for automatic caching.

**Q: Events not firing?**  
A: Subscribe to events **before** emitting them. Check event name matches exactly.

---

## License
This project is part of the R-Type project at EPITECH.

## Authors
**R-Type Development Team** - EPITECH 2025  
Project Repository: [R-Type](https://github.com/ColAntoine/R-Type)

---

<div align="center">
  <strong>Built with modern C++17 for high-performance game development</strong>
  <br>
  <sub>© 2025 R-Type Team - EPITECH</sub>
</div>