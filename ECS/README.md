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
The ECS implementation in this project is modular and supports dynamic loading of components. Here's an overview of the key files:

### 1. `main.cpp`
- **Purpose**: Entry point of the application.
- **Responsibilities**:
  - Initializes the ECS registry.
  - Dynamically loads components from the `libcomponents.so` shared library.
  - Sets up the scene by creating entities and attaching components.
  - Runs the game loop, which includes event handling, updating systems, and rendering.

### 2. `include/ecs/registry.hpp`
- **Purpose**: Core of the ECS framework.
- **Responsibilities**:
  - Manages entities and their associated components.
  - Provides methods to add, remove, and query components for entities.
  - Allows systems to iterate over entities with specific components.

### 3. `include/ecs/components.hpp`
- **Purpose**: Defines the components used in the ECS.
- **Components**:
  - `position`: Stores the x and y coordinates of an entity.
  - `velocity`: Stores the velocity (vx, vy) of an entity.
  - `drawable`: Stores rendering properties (e.g., color, size).
  - `controllable`: Stores input-related properties (e.g., speed).
  - `collider`: Stores collision properties (e.g., width, height, offsets).

### 4. `include/ecs/systems.hpp`
- **Purpose**: Defines the systems that operate on entities.
- **Systems**:
  - `control_system`: Handles user input and updates entity velocities.
  - `collision_system`: Detects and resolves collisions between entities.
  - `draw_system`: Renders entities with the `drawable` component.

### 5. `src/components/library.cpp`
- **Purpose**: Implements the components and provides dynamic loading functionality.
- **Responsibilities**:
  - Implements the `register_components` function to register all components with the ECS registry.
  - Implements the `IComponentFactory` interface to dynamically create components.

### 6. `include/ecs/component_factory.hpp`
- **Purpose**: Defines the `IComponentFactory` interface.
- **Responsibilities**:
  - Provides methods to dynamically create components without exposing their concrete types.

### 7. `include/ecs/zipper.hpp`
- **Purpose**: Provides a utility for iterating over multiple components simultaneously.
- **How It Works**:
  - The `zipper` utility combines multiple component arrays into a single iterable range.
  - It ensures that only entities with all the specified components are included in the iteration.

---

## Dynamic Loading
The ECS supports dynamic loading of components from a shared library (`libcomponents.so`). This allows you to add or modify components without recompiling the entire application.

### How It Works
1. **Shared Library**:
   - The `libcomponents.so` library contains the implementation of components and the `register_components` function.
   - It also provides a factory (`IComponentFactory`) for creating components dynamically.

2. **Dynamic Loading**:
   - The `main.cpp` file uses `dlopen` to load the shared library at runtime.
   - The `register_components` function is called to register all components with the ECS registry.
   - The `get_component_factory` function is used to retrieve the factory for creating components.

---

## Example Workflow
### 1. Adding a New Component
To add a new component:
1. Define the component in `components.hpp`.
2. Implement the component in `library.cpp`.
3. Update the `register_components` function to register the new component.

### 2. Adding a New System
To add a new system:
1. Define the system in `systems.hpp`.
2. Implement the system logic in `systems.cpp`.
3. Call the system in the `update` or `render` function in `main.cpp`.

---

## Code Example
### Creating an Entity with Components
```cpp
auto entity = reg.spawn_entity();
reg.emplace_component<position>(entity, 100.f, 200.f);
reg.emplace_component<velocity>(entity, 5.f, 0.f);
reg.emplace_component<drawable>(entity, 50.f, 50.f, 255, 0, 0, 255);
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

### Example 1: Adding a Controllable Entity
```cpp
auto player = reg.spawn_entity();
reg.emplace_component<position>(player, 50.f, 50.f);
reg.emplace_component<velocity>(player, 0.f, 0.f);
reg.emplace_component<drawable>(player, 20.f, 20.f, 0, 255, 0, 255);
reg.emplace_component<controllable>(player, 150.f);
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

### Example 3: Rendering Entities
```cpp
for (auto [pos, draw] : zipper(reg.get<position>(), reg.get<drawable>())) {
    sf::RectangleShape shape(sf::Vector2f(draw.w, draw.h));
    shape.setPosition(pos.x, pos.y);
    shape.setFillColor(sf::Color(draw.r, draw.g, draw.b, draw.a));
    window.draw(shape);
}
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