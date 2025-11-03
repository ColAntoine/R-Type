<div align="center">

# ⚔️ R-Type - Multiplayer Space Shooter

![C++](https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus)
![License](https://img.shields.io/badge/license-EPITECH-green)
![Status](https://img.shields.io/badge/status-active-brightgreen)
![Platform](https://img.shields.io/badge/platform-cross--platform-lightgrey)

A **modern, high-performance** implementation of the classic R-Type space shooter with real-time multiplayer support. Built with clean architecture featuring **Entity-Component-System**, **service-oriented design**, and **asynchronous networking**.

[🎮 Play](#quick-start) • [📖 Documentation](#documentation) • [🛠️ Develop](#development) • [👥 Contribute](#contributing)

</div>

---

## ✨ Features

### Gameplay
- ⚔️ **Real-time multiplayer** — 1-4 players 
- 👾 **6 enemy types** with unique AI behaviors (Basic, Sine Wave, Fast, Zigzag, Turret, Boss)
- 💥 **Dynamic weapon system** with multiple projectile types (Hard Bullets, Big Bullets, Parabol Shots, Explosions)
- 🌊 **Wave-based progression** with increasing difficulty
- 🎨 **Smooth animations** and visual effects
- ⚡ **Power-ups** — Speed, Fire Rate, Damage upgrades

### Engine & Architecture
- 🏗️ **ECS Pattern** — Modular entity-component-system with dynamic plugin loading
- 🔌 **Service-Oriented Design** — Dependency injection for scalability
- ⚡ **Event-Driven Communication** — Decoupled systems
- 🌐 **UDP Networking** — Low-latency real-time multiplayer
- 📡 **Binary Protocol** — Custom KPN v1.0 efficient protocol
- 🎮 **Client-Side Prediction** — Responsive controls with server reconciliation

### Developer Experience
- 📚 **Comprehensive Documentation** — For players and developers
- 🛠️ **Plugin System** — Create custom components & systems
- 🎓 **Well-Structured Code** — Clear separation of concerns
- 🚀 **Cross-Platform** — Linux, macOS, Windows support

---

## 🚀 Quick Start

### Prerequisites

Before you begin, ensure you have the following installed:

| Requirement | Version | Purpose |
|---|---|---|
| **C++ Compiler** | GCC 7+, Clang 5+, MSVC 2017+ | Compile C++17 code |
| **CMake** | 3.10+ | Build system |
| **Git** | Latest | Clone repository |
| **vcpkg** | Latest | Dependency management |

> **💡 Platform Support**: Linux, macOS, Windows

### Installation & Build

```bash
# 1. Clone the repository
git clone https://github.com/ColAntoine/R-Type.git
cd R-Type

# 2. Initialize vcpkg (handles all C++ dependencies automatically)
./external/vcpkg/bootstrap-vcpkg.sh  # Linux/macOS
# or
.\external\vcpkg\bootstrap-vcpkg.bat  # Windows

# 3. Build the entire project
make

# 4. Run the game
./r-type_client              # Launch game
```

> **Note**: First build may take time as vcpkg installs dependencies (Raylib, Asio, etc.)

### Run Locally

```bash
# Terminal 1: Start server
./r-type_server

# Terminal 2+: Launch game clients
./r-type_client
./r-type_client
```

---

## 📖 Documentation

### For Players

👉 **[Games/RType/README.md](Games/RType/README.md)** — Complete gameplay guide, controls, multiplayer setup

### For Developers

Choose your focus area:

| Goal | Documentation |
|---|---|
| **Understand ECS architecture** | [📘 ECS/README.md](ECS/README.md) |
| **Build with R-Type framework** | [📙 Games/RType/README.md](Games/RType/README.md) |
| **Create a new game** | [📕 Games/README.md](Games/README.md) |
| **Work on networking** | [📗 Games/RType/Networking.md](Games/RType/Networking.md) |
| **Protocol details** | [📓 Protocol.md](Protocol.md) |
| **UI system guide** | [📔 ECS/include/ECS/UI/UIBuilder_DOCUMENTATION.md](ECS/include/ECS/UI/UIBuilder_DOCUMENTATION.md) |

---

## 🏗️ Project Structure

```
R-Type/
├── 📦 ECS/                           Entity-Component-System library (core engine)
│   ├── include/ECS/                  Core ECS headers
│   │   ├── Components/               Base components (position, velocity, sprite, etc.)
│   │   ├── Systems/                  Base systems (animation, collision, etc.)
│   │   ├── Renderer/                 Graphics rendering system
│   │   ├── Physics/                  Physics & collision detection
│   │   ├── Audio/                    Sound & audio system
│   │   ├── AssetManager/             Asset loading & management
│   │   ├── UI/                       UI building & components
│   │   ├── Messaging/                Event/message system
│   │   └── Registry.hpp              Entity & component management
│   ├── src/                          ECS implementation & systems
│   ├── test/                         ECS unit tests
│   └── README.md                     ⭐ ECS technical documentation
│
├── 🎮 Games/                         Multi-game framework
│   ├── RType/                        R-Type game implementation
│   │   ├── Core/                     Framework foundation
│   │   │   ├── Client/               Client-side logic
│   │   │   ├── Server/               Server-side logic
│   │   │   ├── States/               Game state machine
│   │   │   ├── Config/               Configuration & constants
│   │   │   └── KeyBindingManager/    Input handling
│   │   ├── Entity/                   Game-specific ECS extensions
│   │   │   ├── Components/           Game components (Enemy, Weapon, PowerUp, etc.)
│   │   │   └── Systems/              Game systems (AI, Shooting, etc.)
│   │   ├── UI/                       Game UI screens
│   │   ├── Assets/                   Game assets & resources
│   │   ├── main_client.cpp           Client entry point
│   │   ├── main_server.cpp           Server entry point
│   │   ├── Networking.md             ⭐ Networking guide
│   │   └── README.md                 ⭐ Game documentation
│   │
│   ├── Pang/                         Example game (reference implementation)
│   ├── test/                         Game tests
│   └── README.md                     ⭐ Multi-game framework guide
│
├── 🌐 r-type_server                  Server executable (UDP/KPN)
├── 🎮 r-type_client                  Client executable (Raylib)
│
├── 📋 Protocol.md                    ⭐ KPN v1.0 protocol specification
├── CMakeLists.txt                    Root build configuration
├── Makefile                          Build commands
├── vcpkg.json                        Dependency manifest
└── scripts/                          Build & utility scripts
```

### Core Components

**ECS/ (Engine Foundation)**
- `Registry.hpp` — Entity/Component management with SparseSet storage
- `Components/` — Base components (Position, Velocity, Sprite, CollisionBox, etc.)
- `Systems/` — Base systems (Animation, Physics, Rendering, etc.)
- `Renderer/` — Raylib graphics abstraction
- `Physics/` — Collision detection & response
- `Audio/` — Sound management
- `UI/` — Button, Text, Input UI components

**Games/RType/ (Game Implementation)**
- `Core/Client/` — Game client logic (state machine, input, rendering)
- `Core/Server/` — Game server logic (connections, game simulation)
- `Core/States/` — Game states (MainMenu, Lobby, WaitingLobby, InGame, etc.)
- `Entity/Components/` — Enemy, Weapon, Player, PowerUp, etc.
- `Entity/Systems/` — EnemySpawn, Shooting, Collision, etc.

**Networking**
- UDP-based server using Asio
- Custom KPN v1.0 binary protocol (defined in Protocol.md)
- Client-server synchronization
- Lobby system for multiplayer

---

## 🔧 Development

### Getting Started

```bash
# 1. Clone and setup
git clone https://github.com/ColAntoine/R-Type.git
cd R-Type

# 2. Build everything
make

# 3. Run locally
./r-type_server      # Terminal 1
./r-type_client      # Terminal 2+
```

### Directory Guide

**Want to modify gameplay?**
- 👾 Enemy behavior → `Games/RType/Entity/Systems/`
- 💥 Weapons/projectiles → `Games/RType/Entity/Components/Weapon/`
- ⚡ Power-ups → `Games/RType/Entity/Components/PowerUp/`

**Want to extend the engine?**
- 🏗️ Add components → `ECS/include/ECS/Components/`
- ⚙️ Add systems → `ECS/src/` (or games can add custom systems)
- 🎨 Rendering features → `ECS/src/Renderer/`

**Want to work on networking?**
- 🔌 Protocol changes → `Games/RType/Core/Client/` or `Core/Server/`
- 📡 Message handling → See `Protocol.md`

**Want to create a new game?**
- 📁 Create `Games/YourGame/` following RType structure
- 🎯 See [Games/README.md](Games/README.md) for framework overview

---

## 👥 Credits

This project was developed by the **EPITECH R-Type Team**:

- **💻 Core Team** — ColAntoine, Sumerinsuyu, KllrMaxime, Onillpyd and gabriel-thuus
- **🎮 Game Design** — Classic arcade spirit with modern multiplayer twist
- **📚 Documentation** — Comprehensive guides for players and developers

---

## 🚀 Contributing

We welcome contributions! Whether you're a developer, game designer, or just enthusiastic:

### How to Contribute

1. **Fork** the repository
2. **Create** a feature branch: `git checkout -b feature/amazing-feature`
3. **Code** following our architecture patterns
4. **Document** your changes
5. **Test** thoroughly
6. **Submit** a Pull Request

### What We're Looking For

- 🆕 **New Features** — Weapons, enemies, gameplay mechanics
- 🐛 **Bug Fixes** — Found an issue? Help us fix it!
- 📖 **Documentation** — Improve guides and code comments
- ⚡ **Performance** — Optimize hot paths
- 🌐 **Networking** — Enhance multiplayer experience
- 🎨 **UI/UX** — Create beautiful interfaces

### Code Style

- Use `snake_case` for variables and functions
- Use `PascalCase` for classes and types
- Write comments for non-obvious logic
- Follow existing architecture patterns
- 4-space indentation

---

## 📋 Project Goals

This project implements the **EPITECH R-Type curriculum** with:

- ✅ Modern C++17 architecture
- ✅ High-performance ECS engine
- ✅ Real-time multiplayer networking
- ✅ Binary protocol optimization
- ✅ Cross-platform compatibility
- ✅ Extensible framework for multiple games

---

## 📄 License

This project is part of the **EPITECH curriculum**. Educational use only.

<div align="center">

**[⬆ back to top](#-r-type--multiplayer-space-shooter)**

---

Made with ❤️ by the R-Type Development Team

**[🌍 GitHub](https://github.com/ColAntoine/R-Type)** • **[📚 EPITECH](https://www.epitech.eu/)**

</div>

---
