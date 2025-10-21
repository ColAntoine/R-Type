# Pang Game - ECS Implementation

A Pang game built using the custom ECS (Entity Component System) architecture.

## 📁 Project Structure

```
Pang/
├── CMakeLists.txt          # Build configuration
├── Makefile               # Build wrapper
├── main.cpp              # Game entry point
├── README.md             # This file
├── Assets/               # Game assets (textures, sounds, etc.)
├── Core/                 # Core game logic
└── Entity/               # ECS entities
    ├── Components/       # Game-specific components
    └── Systems/          # Game systems (compiled as .so)
```

## 🔧 Building the Game

### Prerequisites
- CMake 3.21+
- C++20 compatible compiler
- raylib (installed via vcpkg)
- ECS library (from parent project)

### Build Commands

```bash
# Build the game
make

# Clean build artifacts
make clean

# Full clean (including CMake cache)
make fclean

# Rebuild from scratch
make re

# Build and run
make run

# Build and debug with GDB
make debug
```

## 🎯 Architecture

### ECS Integration

This game uses the shared ECS library from `../../ECS`:
- **Components**: Position, Velocity, Sprite, Collider, etc.
- **Systems**: Dynamically loaded from `.so` libraries
- **Registry**: Central entity and component manager

### System Libraries

All systems in `Entity/Systems/` are compiled as shared libraries:
- `libpang_PlayerControl.so` - Player input and movement
- `libpang_BallPhysics.so` - Ball movement and splitting
- `libpang_Collision.so` - Collision detection
- etc.

These libraries are loaded at runtime and can be hot-reloaded during development.

### Components

Game-specific components are in `Entity/Components/`:
- Each component is a simple data structure
- Components are registered with the ECS registry
- Multiple components can be attached to entities

## 🚀 Development

### Adding a New System

1. Create a directory in `Entity/Systems/YourSystem/`
2. Create `YourSystem.cpp` with the system implementation
3. Implement the system interface from ECS
4. Rebuild - CMake will automatically compile it as a library

Example:
```cpp
// Entity/Systems/MySystem/MySystem.cpp
#include "ECS/Systems/ISystem.hpp"
#include "ECS/Registry.hpp"

extern "C" {
    ISystem* create_system() {
        return new MySystem();
    }
    
    void destroy_system(ISystem* system) {
        delete system;
    }
}

class MySystem : public ISystem {
    void update(registry& reg, float deltaTime) override {
        // Your system logic here
    }
};
```

### Adding a New Component

1. Create header in `Entity/Components/YourComponent.hpp`
2. Create implementation in `Entity/Components/YourComponent.cpp` (if needed)
3. Register the component in the game initialization

## 📝 Key Differences from R-Type

- **No Networking**: Pure local gameplay
- **Independent**: Can be built and run standalone
- **Simpler**: Focus on core game mechanics

## 🐛 Debugging

```bash
# Run with GDB
make debug

# Check loaded systems
ldd ./pang

# View system libraries
ls -la lib/systems/libpang_*.so
```

## 📚 Resources

- [ECS Documentation](../../ECS/README.md)
- [raylib Documentation](https://www.raylib.com/)
- [Original Pang Game](https://en.wikipedia.org/wiki/Pang_(video_game))

## 🎓 Purpose

This project demonstrates the flexibility and reusability of the ECS architecture - the same framework used for R-Type can be used to build completely different games without any networking dependencies.

---

**Ready to build your game!** The structure is set up, now implement your components and systems.
