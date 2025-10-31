# Entity Component System (ECS) Documentation

## Overview
The **Entity Component System (ECS)** is a design pattern commonly used in game development and simulations. It provides a flexible and efficient way to manage entities and their associated data (components) while decoupling logic (systems) from data.

### Key Concepts
1. **Entity**:
   - Represents a unique object in the world (e.g., a player, an enemy, or a projectile).
   - Entities are essentially IDs (e.g., integers) and do not contain any data themselves.

2. **Component**:
   - Represents a piece of data or state associated with an entity (e.g., position, velocity, health).
   - Components are plain data structures (PODs) and do not contain logic.

3. **System**:
   - Contains the logic that operates on entities with specific components.
   - Systems iterate over entities that have the required components and perform operations on them.

---

## Project Structure
The ECS implementation in this project is modular and supports dynamic loading of both components and systems. Here's an overview of the key files:

### 1. `include/ecs/registry.hpp`
- **Purpose**: Core of the ECS framework.
- **Responsibilities**:
  - Manages entities and their associated components.
  - Provides methods to add, remove, and query components for entities.
  - Allows systems to iterate over entities with specific components.

### 2. `include/ecs/components.hpp`
- **Purpose**: Defines the components used in the ECS.
- **Available Components**:
  - `position`: Stores the x and y coordinates of an entity.
  - `velocity`: Stores the velocity (vx, vy) of an entity.
  - `sprite`: Stores rendering properties (texture path, dimensions, scale, rotation, frame coordinates, visibility).
  - `animation`: Stores spritesheet animation data (texture path, frame dimensions, frame count, timing, loop settings).
  - `collider`: Stores collision properties (width, height, offsets, trigger flag).

### 3. `include/ecs/systems/` (System Headers)
- **Purpose**: Defines the interface for systems that operate on entities.
- **Available Systems**:
  - `position_system`: Updates entity positions based on velocity.
  - `collision_system`: Detects and resolves collisions between entities with collider components.
  - `sprite_system`: Renders entities with sprite components.
  - `animation_system`: Manages spritesheet animations and frame progression.

### 4. `src/components/library.cpp`
- **Purpose**: Implements the components and provides dynamic loading functionality.
- **Responsibilities**:
  - Implements the `register_components` function to register all components with the ECS registry.
  - Implements the `IComponentFactory` interface to dynamically create components.

### 5. `include/ecs/component_factory.hpp`
- **Purpose**: Defines the `IComponentFactory` interface.
- **Responsibilities**:
  - Provides methods to dynamically create components without exposing their concrete types.

### 6. `include/ecs/ILoader.hpp`
- **Purpose**: Handles dynamic loading of shared libraries.
- **Responsibilities**:
  - Loads component libraries (`libECS.so`) at runtime.
  - Loads system libraries (e.g., `libposition_system.so`, `libcollision_system.so`, `libsprite_system.so`, `libanimation_system.so`).
  - Provides interface for registering components and systems with the registry.

### 7. `include/ecs/zipper.hpp`
- **Purpose**: Provides a utility for iterating over multiple components simultaneously.
- **How It Works**:
  - The `zipper` utility combines multiple component arrays into a single iterable range.
  - It ensures that only entities with all the specified components are included in the iteration.

---

## Dynamic Loading Architecture
The ECS library is built as a framework that supports dynamic loading of both components and systems through shared libraries. This allows you to add, modify, or replace components and systems without recompiling the core ECS library.

### Component Libraries
1. **Component Shared Library (`libECS.so`)**:
   - Contains the implementation of all components.
   - Exports a `register_components` function that registers components with the ECS registry.
   - Provides a factory (`IComponentFactory`) for creating component instances dynamically.

2. **Loading Components**:
   - Use the `ILoader` class to load the shared library at runtime via `dlopen`.
   - Call `register_components` to register all components with the registry.
   - Retrieve the `IComponentFactory` to create component instances dynamically.

### System Libraries
1. **System Shared Libraries** (e.g., `libposition_system.so`, `libcollision_system.so`, `libsprite_system.so`, `libanimation_system.so`):
   - Each system can be compiled as an independent shared library.
   - Systems export their logic through a standardized interface (`ISystem`).
   - Located in `lib/systems/` directory by default.

2. **Loading Systems**:
   - Systems are loaded individually through `ILoader::load_system()`.
   - Each system library provides its initialization and execution functions.
   - Systems can be loaded, unloaded, or replaced at runtime without affecting other systems.

### Integration Example
```cpp
#include "ECS/Registry.hpp"
#include "ECS/ILoader.hpp"

int main() {
    registry reg;
    ILoader loader;
    
    // Load component library
    if (!loader.load_components("./libECS.so", reg)) {
        std::cerr << "Failed to load components!" << std::endl;
        return 1;
    }
    
    // Load system libraries
    loader.load_system_from_so("./lib/systems/libposition_system.so");
    loader.load_system_from_so("./lib/systems/libcollision_system.so");
    loader.load_system_from_so("./lib/systems/libsprite_system.so");
    loader.load_system_from_so("./lib/systems/libanimation_system.so");
    
    // Your game logic here...
    return 0;
}
```

---

## Development Workflows

### Creating a New Component
To add a new component to the ECS:
1. **Define** the component structure in `include/ECS/Components/<ComponentName>.hpp`.
2. **Implement** the component logic in `src/components/<component_name>.cpp`.
3. **Register** the component in `src/components/library.cpp` within the `register_components` function.
4. **Rebuild** the component library (`libECS.so`).

### Creating a New System
To add a new system:
1. **Define** the system interface in `include/ECS/Systems/<SystemName>.hpp` (implement `ISystem`).
2. **Implement** the system logic in `src/systems/<system_name>.cpp`.
3. **Export** the system's initialization and execution functions.
4. **Compile** as a separate shared library (e.g., `lib<system_name>.so`).
5. **Load** the system dynamically using `ILoader` in your application.

### Building Component and System Libraries
```bash
# Build the component library
cd ECS
./build.sh

# Component library will be generated as libECS.so
# System libraries will be in lib/systems/
# libECS.so will be copied in lib/
```

---

## Code Example
### Creating an Entity with Components
```cpp
auto entity = reg.spawn_entity();
reg.emplace_component<position>(entity, 100.f, 200.f);
reg.emplace_component<velocity>(entity, 5.f, 0.f);
reg.emplace_component<sprite>(entity, "assets/player.png", 64.f, 64.f);
reg.emplace_component<collider>(entity, 64.f, 64.f);
```

### Iterating Over Entities in a System
```cpp
for (auto [pos, vel] : zipper(reg.get<position>(), reg.get<velocity>())) {
    pos.x += vel.vx * dt;
    pos.y += vel.vy * dt;
}
```

---

## Zipper Utility Example
The `zipper` utility is used to iterate over entities with multiple components. For example:

```cpp
for (auto [pos, vel] : zipper(reg.get<position>(), reg.get<velocity>())) {
    pos.x += vel.vx * dt;
    pos.y += vel.vy * dt;
}
```

### Explanation:
- `zipper` takes two or more component arrays as arguments.
- It iterates over entities that have all the specified components.
- In the example above:
  - `pos` is the `position` component of an entity.
  - `vel` is the `velocity` component of the same entity.
  - The loop updates the position of each entity based on its velocity and the time step `dt`.

---

## Additional Examples

### Example 1: Creating an Animated Entity
```cpp
auto animated_entity = reg.spawn_entity();
reg.emplace_component<position>(animated_entity, 50.f, 50.f);
reg.emplace_component<velocity>(animated_entity, 0.f, 0.f);
reg.emplace_component<animation>(animated_entity, "assets/spritesheet.png", 64.f, 64.f, 8);
reg.emplace_component<collider>(animated_entity, 64.f, 64.f);
```

### Example 2: Detecting Collisions
```cpp
for (auto [collider1, pos1] : zipper(reg.get<collider>(), reg.get<position>())) {
    for (auto [collider2, pos2] : zipper(reg.get<collider>(), reg.get<position>())) {
        if (&collider1 != &collider2) {
            if (check_collision(pos1, collider1, pos2, collider2)) {
                resolve_collision(pos1, collider1, pos2, collider2);
            }
        }
    }
}
```

### Example 3: Rendering Sprites
```cpp
for (auto [pos, spr] : zipper(reg.get<position>(), reg.get<sprite>())) {
    if (spr.visible) {
        // Load texture and create sprite
        // Position sprite at (pos.x, pos.y)
        // Apply scale (spr.scale_x, spr.scale_y)
        // Apply rotation (spr.rotation)
        // Render the sprite
    }
}
```

### Example 4: Creating a Moving Enemy
```cpp
auto enemy = reg.spawn_entity();
reg.emplace_component<position>(enemy, 700.f, 300.f);
reg.emplace_component<velocity>(enemy, -50.f, 0.f);  // Moving left
reg.emplace_component<sprite>(enemy, "assets/enemy.png", 48.f, 48.f, 1.5f, 1.5f);  // 1.5x scale
reg.emplace_component<collider>(enemy, 48.f, 48.f, 0.f, 0.f, false);
```

---

## Advantages of This ECS
1. **Modularity**:
   - Components and systems are decoupled, making it easy to add or modify functionality.
2. **Dynamic Loading**:
   - Components can be added or updated without recompiling the entire application.
3. **Performance**:
   - The use of sparse arrays and efficient iteration ensures high performance.

---

## Future Improvements
1. **Parallelism**:
   - Systems can be parallelized to improve performance on multi-core processors.
2. **Serialization**:
   - Add support for saving and loading the state of the ECS.
3. **Event System**:
   - Implement an event system for better communication between systems.

---

## Contributing

Contributions are welcome! Please follow these guidelines:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

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

- Inspired by classic ECS implementations
- Built with modern C++ practices
- Designed for high-performance game development

---

<div align="center">
  <strong>Made with ❤️ for game development</strong>
  <br>
  <sub>© 2025 R-Type Team - EPITECH</sub>
</div>