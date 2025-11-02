# Game Prototype Development Guide

## Overview

**Build a complete game prototype from scratch using the R-Type ECS engine.**

This guide walks you through creating a playable game in under 2 hours. You'll learn how to integrate the ECS engine, create entities, implement gameplay systems, and build a complete game loop.

**Target Audience**: 
- **Developers**: C++ programmers wanting to build games with data-oriented architecture
- **Users**: Game designers prototyping gameplay ideas

**What You'll Build**:
A simple space shooter with:
- Player-controlled ship
- Enemies that spawn and move
- Shooting mechanics
- Collision detection
- Score tracking
- Simple UI

**Prerequisites**:
- C++17 compiler
- CMake 3.21+
- Raylib library
- Basic C++ knowledge

**Time to Complete**: ~2 hours

---

## Quick Architecture Overview

Your game will have **3 main layers**:

### 1. **ECS Layer** (Entities, Components, Systems)
- **What it does**: Manages all game objects (player, enemies, bullets)
- **Documentation**: See [`../ECS/README.md`](../ECS/README.md) for full ECS documentation
- **You'll use**: `registry`, components (`position`, `velocity`, `sprite`), systems

### 2. **Game State Layer** (Menu, Gameplay, Game Over)
- **What it does**: Controls game flow and transitions
- **Pattern**: Stack-based state machine
- **You'll create**: `InGame` state with gameplay logic

### 3. **Manager Layer** (Rendering, Audio, Input)
- **What it does**: Handles cross-cutting concerns
- **Documentation**: See [`../ECS/FEATURES.md`](../ECS/FEATURES.md) for manager details
- **You'll use**: `RenderManager`, `AssetManager`, `MessagingManager`

**Data Flow:**
```
Input → Systems → Components → Rendering
  ↓
Events (for communication between systems)
```

---

## Step 0: Setup Your Project (5 minutes)

### Create Project Structure
```bash
mkdir SpaceShooter && cd SpaceShooter
mkdir -p Core/States/InGame
mkdir -p Entity/{Components,Systems}
mkdir -p Assets
touch main.cpp CMakeLists.txt
```

### Final Structure
```
SpaceShooter/
├── CMakeLists.txt          # Build configuration
├── main.cpp                # Entry point
├── Core/
│   └── States/
│       ├── IGameState.hpp  # State interface
│       └── InGame/
│           ├── InGame.hpp
│           └── InGame.cpp  # Main gameplay
├── Entity/
│   ├── Components/         # Custom components
│   │   ├── Player.hpp
│   │   ├── Enemy.hpp
│   │   └── Bullet.hpp
│   └── Systems/            # Custom systems
│       ├── PlayerSystem.cpp
│       ├── EnemySystem.cpp
│       └── BulletSystem.cpp
└── Assets/                 # Textures, sounds
```

### Create CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.21)
project(SpaceShooter VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Link to ECS library
include_directories(../ECS/include)
link_directories(../ECS/build/lib)

# Find Raylib
find_package(raylib REQUIRED)

# Collect source files
file(GLOB_RECURSE SOURCES 
    "main.cpp"
    "Core/**/*.cpp"
    "Entity/**/*.cpp"
)

# Create executable
add_executable(space_shooter ${SOURCES})
target_link_libraries(space_shooter PRIVATE 
    ECS 
    raylib
    dl
)

# Copy assets
file(COPY Assets DESTINATION ${CMAKE_BINARY_DIR})
```

---

## Step 1: Define Your Game Components (10 minutes)

Components are pure data structures. See [`../ECS/README.md#components`](../ECS/README.md) for core components.

### Player Component (`Entity/Components/Player.hpp`)
```cpp
#pragma once
#include "ECS/Components/IComponent.hpp"

struct Player : public IComponent {
    float speed{300.0f};
    int lives{3};
    int score{0};
    float shoot_cooldown{0.5f};
    float current_cooldown{0.0f};
    
    Player() = default;
    Player(float spd, int lv, int sc) 
        : speed(spd), lives(lv), score(sc) {}
};
```

### Enemy Component (`Entity/Components/Enemy.hpp`)
```cpp
#pragma once
#include "ECS/Components/IComponent.hpp"

struct Enemy : public IComponent {
    int type{0};          // 0=basic, 1=fast, 2=tank
    float health{100.0f};
    int score_value{10};
    
    Enemy() = default;
    Enemy(int t, float hp, int score) 
        : type(t), health(hp), score_value(score) {}
};
```

### Bullet Component (`Entity/Components/Bullet.hpp`)
```cpp
#pragma once
#include "ECS/Components/IComponent.hpp"

struct Bullet : public IComponent {
    float damage{25.0f};
    int owner_id{-1};     // -1=player, else=enemy_id
    float lifetime{5.0f};
    float age{0.0f};
    
    Bullet() = default;
    Bullet(float dmg, int owner, float life) 
        : damage(dmg), owner_id(owner), lifetime(life) {}
};
```

**Note**: You'll also use core ECS components:
- `position` (x, y coordinates)
- `velocity` (vx, vy speed)
- `sprite` (texture rendering)
- `collider` (collision box)

See [`../ECS/README.md#components`](../ECS/README.md) for details.

---

## Step 2: Create Game Systems (30 minutes)

Systems contain game logic. See [`../ECS/README.md#systems`](../ECS/README.md) for system patterns

### Player System (`Entity/Systems/PlayerSystem.cpp`)
Handles player input and movement.

```cpp
#include "ECS/Systems/ISystem.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "Entity/Components/Player.hpp"
#include "Entity/Components/Bullet.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include <raylib.h>

class PlayerSystem : public ISystem {
public:
    void update(registry& reg, float dt) override {
        auto* positions = reg.get_if<position>();
        auto* velocities = reg.get_if<velocity>();
        auto* players = reg.get_if<Player>();
        
        if (!positions || !velocities || !players) return;
        
        for (auto [pos, vel, player, idx] : zipper(*positions, *velocities, *players)) {
            // Update cooldown
            if (player.current_cooldown > 0.0f) {
                player.current_cooldown -= dt;
            }
            
            // Input handling
            vel.vx = 0;
            vel.vy = 0;
            
            if (IsKeyDown(KEY_LEFT))  vel.vx = -player.speed;
            if (IsKeyDown(KEY_RIGHT)) vel.vx = player.speed;
            if (IsKeyDown(KEY_UP))    vel.vy = -player.speed;
            if (IsKeyDown(KEY_DOWN))  vel.vy = player.speed;
            
            // Shooting
            if (IsKeyPressed(KEY_SPACE) && player.current_cooldown <= 0.0f) {
                spawn_bullet(reg, pos.x, pos.y, static_cast<int>(idx));
                player.current_cooldown = player.shoot_cooldown;
            }
            
            // Boundary check
            auto screen = RenderManager::instance().get_screen_infos();
            pos.x = std::clamp(pos.x, 0.0f, (float)screen.getWidth());
            pos.y = std::clamp(pos.y, 0.0f, (float)screen.getHeight());
        }
    }
    
    const char* get_name() const override { return "PlayerSystem"; }

private:
    void spawn_bullet(registry& reg, float x, float y, int owner_id) {
        entity bullet = reg.spawn_entity();
        reg.emplace_component<position>(bullet, x, y - 20.0f);
        reg.emplace_component<velocity>(bullet, 0.0f, -500.0f);  // Move up
        reg.emplace_component<Bullet>(bullet, 25.0f, owner_id, 5.0f);
        reg.emplace_component<sprite>(bullet, "Assets/bullet.png", 8.0f, 16.0f);
        reg.emplace_component<collider>(bullet, 8.0f, 16.0f);
    }
};

// Export for dynamic loading
extern "C" {
    ISystem* create_system() { return new PlayerSystem(); }
    void destroy_system(ISystem* sys) { delete sys; }
}
```

### Enemy System (`Entity/Systems/EnemySystem.cpp`)
Spawns and updates enemies.

```cpp
#include "ECS/Systems/ISystem.hpp"
#include "Entity/Components/Enemy.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/Zipper.hpp"
#include <random>

class EnemySystem : public ISystem {
private:
    float spawn_timer_{0.0f};
    float spawn_interval_{2.0f};
    std::mt19937 rng_{std::random_device{}()};

public:
    void update(registry& reg, float dt) override {
        // Spawn enemies periodically
        spawn_timer_ += dt;
        if (spawn_timer_ >= spawn_interval_) {
            spawn_enemy(reg);
            spawn_timer_ = 0.0f;
        }
        
        // Move enemies down
        auto* positions = reg.get_if<position>();
        auto* velocities = reg.get_if<velocity>();
        auto* enemies = reg.get_if<Enemy>();
        
        if (!positions || !velocities || !enemies) return;
        
        std::vector<entity> to_remove;
        for (auto [pos, vel, enemy, idx] : zipper(*positions, *velocities, *enemies)) {
            // Remove if off-screen
            if (pos.y > 1080.0f) {
                to_remove.push_back(static_cast<entity>(idx));
            }
        }
        
        for (auto e : to_remove) {
            reg.kill_entity(e);
        }
    }
    
    const char* get_name() const override { return "EnemySystem"; }

private:
    void spawn_enemy(registry& reg) {
        std::uniform_real_distribution<float> x_dist(50.0f, 1870.0f);
        float x = x_dist(rng_);
        
        entity enemy = reg.spawn_entity();
        reg.emplace_component<position>(enemy, x, -50.0f);
        reg.emplace_component<velocity>(enemy, 0.0f, 150.0f);  // Move down
        reg.emplace_component<Enemy>(enemy, 0, 100.0f, 10);
        reg.emplace_component<sprite>(enemy, "Assets/enemy.png", 64.0f, 64.0f);
        reg.emplace_component<collider>(enemy, 64.0f, 64.0f);
    }
};

extern "C" {
    ISystem* create_system() { return new EnemySystem(); }
    void destroy_system(ISystem* sys) { delete sys; }
}
```

### Bullet System (`Entity/Systems/BulletSystem.cpp`)
Updates bullets and removes old ones.

```cpp
#include "ECS/Systems/ISystem.hpp"
#include "Entity/Components/Bullet.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Zipper.hpp"

class BulletSystem : public ISystem {
public:
    void update(registry& reg, float dt) override {
        auto* bullets = reg.get_if<Bullet>();
        auto* positions = reg.get_if<position>();
        
        if (!bullets || !positions) return;
        
        std::vector<entity> to_remove;
        for (auto [bullet, pos, idx] : zipper(*bullets, *positions)) {
            bullet.age += dt;
            
            // Remove if too old or off-screen
            if (bullet.age >= bullet.lifetime || 
                pos.y < -50.0f || pos.y > 1130.0f) {
                to_remove.push_back(static_cast<entity>(idx));
            }
        }
        
        for (auto e : to_remove) {
            reg.kill_entity(e);
        }
    }
    
    const char* get_name() const override { return "BulletSystem"; }
};

extern "C" {
    ISystem* create_system() { return new BulletSystem(); }
    void destroy_system(ISystem* sys) { delete sys; }
}
```

### Collision System (Use ECS built-in or custom)
For collision detection, you have two options:

**Option A**: Use ECS PhysicsManager (recommended, O(n) performance)
```cpp
// See ../ECS/FEATURES.md#feature-3-physics--collision
auto& physics = PhysicsManager::instance();
physics.init(64.0f);
// Update entities and get collision pairs
```

**Option B**: Simple O(n²) collision (quick prototype)
```cpp
// Check all bullets against all enemies
for (auto [bullet_pos, bullet_col, bullet_data] : /* bullet entities */) {
    for (auto [enemy_pos, enemy_col, enemy_data] : /* enemy entities */) {
        if (check_aabb_collision(bullet_pos, bullet_col, enemy_pos, enemy_col)) {
            // Handle collision
        }
    }
}
```

See [`../ECS/FEATURES.md#feature-3-physics--collision`](../ECS/FEATURES.md) for detailed physics documentation

---

## Step 3: Create Game State (20 minutes)

States control game flow. We'll create a simple InGame state.

### State Interface (`Core/States/IGameState.hpp`)
```cpp
#pragma once

class IGameState {
public:
    virtual ~IGameState() = default;
    
    virtual void enter() = 0;      // Called when state starts
    virtual void exit() = 0;       // Called when state ends
    virtual void update(float dt) = 0;  // Called every frame
    
    virtual std::string get_name() const = 0;
};
```

### InGame State (`Core/States/InGame/InGame.hpp`)
```cpp
#pragma once
#include "Core/States/IGameState.hpp"
#include "ECS/Registry.hpp"
#include "ECS/ILoader.hpp"
#include "ECS/LinuxLoader.hpp"
#include "Entity/Components/Player.hpp"
#include <memory>

class InGameState : public IGameState {
private:
    registry registry_;
    PlatformLoader loader_;
    entity player_entity_;
    int score_{0};

public:
    void enter() override;
    void exit() override;
    void update(float dt) override;
    std::string get_name() const override { return "InGame"; }

private:
    void setup_ecs();
    void create_player();
    void update_ui();
};
```

### InGame State Implementation (`Core/States/InGame/InGame.cpp`)
```cpp
#include "InGame.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Velocity.hpp"
#include "ECS/Components/Sprite.hpp"
#include "ECS/Components/Collider.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include "ECS/AssetManager/AssetManager.hpp"
#include <iostream>

void InGameState::enter() {
    std::cout << "[InGame] Entering state" << std::endl;
    
    // Initialize managers
    AssetManager::instance().init();
    RenderManager::instance().init("Space Shooter");
    
    // Setup ECS
    setup_ecs();
    
    // Create player
    create_player();
}

void InGameState::exit() {
    std::cout << "[InGame] Exiting state" << std::endl;
    
    // Cleanup
    AssetManager::instance().shutdown();
}

void InGameState::setup_ecs() {
    // Load ECS library
    if (!loader_.load_components("../ECS/build/lib/libECS.so", registry_)) {
        throw std::runtime_error("Failed to load ECS components");
    }
    
    // Load systems
    loader_.load_system("../ECS/build/lib/systems/libposition_system.so", 
                       ILoader::LogicSystem);
    loader_.load_system("../ECS/build/lib/systems/libsprite_system.so", 
                       ILoader::RenderSystem);
    
    // Load custom systems
    loader_.load_system("./build/libplayer_system.so", ILoader::LogicSystem);
    loader_.load_system("./build/libenemy_system.so", ILoader::LogicSystem);
    loader_.load_system("./build/libbullet_system.so", ILoader::LogicSystem);
}

void InGameState::create_player() {
    auto factory = loader_.get_factory();
    
    player_entity_ = registry_.spawn_entity();
    
    // Use factory to create components
    factory->create_component<position>(registry_, player_entity_, 960.0f, 900.0f);
    factory->create_component<velocity>(registry_, player_entity_, 0.0f, 0.0f);
    factory->create_component<Player>(registry_, player_entity_, 300.0f, 3, 0);
    factory->create_component<sprite>(registry_, player_entity_, 
                                     "Assets/player.png", 64.0f, 64.0f);
    factory->create_component<collider>(registry_, player_entity_, 
                                       64.0f, 64.0f, -32.0f, -32.0f);
}

void InGameState::update(float dt) {
    // Update ECS systems
    loader_.update_all_systems(registry_, dt, ILoader::LogicSystem);
    
    // Render
    auto& renderer = RenderManager::instance();
    renderer.begin_frame();
        loader_.update_all_systems(registry_, dt, ILoader::RenderSystem);
        update_ui();
    renderer.end_frame();
    
    // Check game over
    auto* players = registry_.get_if<Player>();
    if (players) {
        for (auto [player, idx] : zipper(*players)) {
            score_ = player.score;
            if (player.lives <= 0) {
                std::cout << "Game Over! Final Score: " << score_ << std::endl;
                // Transition to game over state
            }
        }
    }
}

void InGameState::update_ui() {
    // Draw score
    DrawText(TextFormat("Score: %d", score_), 10, 10, 32, WHITE);
    
    // Draw lives
    auto* players = registry_.get_if<Player>();
    if (players) {
        for (auto [player, idx] : zipper(*players)) {
            DrawText(TextFormat("Lives: %d", player.lives), 10, 50, 32, WHITE);
        }
    }
}
```

---

## Step 4: Create Main Entry Point (10 minutes)

### Main File (`main.cpp`)
```cpp
#include "Core/States/InGame/InGame.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include <iostream>
#include <raylib.h>

int main(int argc, char* argv[]) {
    std::cout << "=== Space Shooter ===" << std::endl;
    
    try {
        // Create game state
        InGameState game;
        
        // Enter game
        game.enter();
        
        // Game loop
        while (!RenderManager::instance().should_close()) {
            float dt = GetFrameTime();
            game.update(dt);
        }
        
        // Cleanup
        game.exit();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Game closed successfully" << std::endl;
    return 0;
}
```

### Build Systems as Shared Libraries

Update `CMakeLists.txt` to build your systems:
```cmake
# ... existing config ...

# Build custom systems as shared libraries
add_library(player_system SHARED Entity/Systems/PlayerSystem.cpp)
target_link_libraries(player_system PRIVATE ECS raylib)

add_library(enemy_system SHARED Entity/Systems/EnemySystem.cpp)
target_link_libraries(enemy_system PRIVATE ECS raylib)

add_library(bullet_system SHARED Entity/Systems/BulletSystem.cpp)
target_link_libraries(bullet_system PRIVATE ECS raylib)
```

---

## Step 5: Build and Run (5 minutes)

### Build the Game
```bash
cd SpaceShooter
mkdir build && cd build
cmake ..
make -j4
```

### Run the Game
```bash
./space_shooter
```

### Verify It Works
You should see:
- ✅ Window opens (1920x1080)
- ✅ Player ship at bottom center
- ✅ Arrow keys move the player
- ✅ Space bar shoots bullets upward
- ✅ Enemies spawn from top and move down
- ✅ Score displayed top-left
- ✅ Lives displayed below score

**Troubleshooting:**

| Problem | Solution |
|---------|----------|
| "Failed to load ECS components" | Check path to `libECS.so` |
| "Texture not found" | Create `Assets/` folder with placeholder images |
| Segfault on startup | Ensure all systems are properly exported with `extern "C"` |
| Systems not updating | Check system is loaded with correct `SystemType` |

---

## Step 6: Add Collision Detection (15 minutes)

### Simple Collision System (`Entity/Systems/CollisionSystem.cpp`)
```cpp
#include "ECS/Systems/ISystem.hpp"
#include "Entity/Components/Player.hpp"
#include "Entity/Components/Enemy.hpp"
#include "Entity/Components/Bullet.hpp"
#include "ECS/Components/Position.hpp"
#include "ECS/Components/Collider.hpp"
#include "ECS/Zipper.hpp"

class CollisionSystem : public ISystem {
public:
    void update(registry& reg, float dt) override {
        check_bullet_enemy_collision(reg);
        check_player_enemy_collision(reg);
    }
    
    const char* get_name() const override { return "CollisionSystem"; }

private:
    void check_bullet_enemy_collision(registry& reg) {
        auto* bullet_positions = reg.get_if<position>();
        auto* bullet_colliders = reg.get_if<collider>();
        auto* bullets = reg.get_if<Bullet>();
        
        auto* enemy_positions = reg.get_if<position>();
        auto* enemy_colliders = reg.get_if<collider>();
        auto* enemies = reg.get_if<Enemy>();
        auto* players = reg.get_if<Player>();
        
        if (!bullet_positions || !bullets || !enemy_positions || !enemies) return;
        
        std::vector<entity> bullets_to_remove;
        std::vector<entity> enemies_to_remove;
        
        for (auto [b_pos, b_col, bullet, b_idx] : 
             zipper(*bullet_positions, *bullet_colliders, *bullets)) {
            
            for (auto [e_pos, e_col, enemy, e_idx] : 
                 zipper(*enemy_positions, *enemy_colliders, *enemies)) {
                
                if (check_aabb(b_pos, b_col, e_pos, e_col)) {
                    // Bullet hit enemy
                    enemy.health -= bullet.damage;
                    bullets_to_remove.push_back(static_cast<entity>(b_idx));
                    
                    if (enemy.health <= 0) {
                        enemies_to_remove.push_back(static_cast<entity>(e_idx));
                        
                        // Add score to player
                        if (players) {
                            for (auto [player, p_idx] : zipper(*players)) {
                                player.score += enemy.score_value;
                            }
                        }
                    }
                }
            }
        }
        
        // Remove dead entities
        for (auto e : bullets_to_remove) reg.kill_entity(e);
        for (auto e : enemies_to_remove) reg.kill_entity(e);
    }
    
    void check_player_enemy_collision(registry& reg) {
        auto* player_positions = reg.get_if<position>();
        auto* player_colliders = reg.get_if<collider>();
        auto* players = reg.get_if<Player>();
        
        auto* enemy_positions = reg.get_if<position>();
        auto* enemy_colliders = reg.get_if<collider>();
        auto* enemies = reg.get_if<Enemy>();
        
        if (!player_positions || !players || !enemy_positions || !enemies) return;
        
        std::vector<entity> enemies_to_remove;
        
        for (auto [p_pos, p_col, player, p_idx] : 
             zipper(*player_positions, *player_colliders, *players)) {
            
            for (auto [e_pos, e_col, enemy, e_idx] : 
                 zipper(*enemy_positions, *enemy_colliders, *enemies)) {
                
                if (check_aabb(p_pos, p_col, e_pos, e_col)) {
                    // Enemy hit player
                    player.lives--;
                    enemies_to_remove.push_back(static_cast<entity>(e_idx));
                }
            }
        }
        
        for (auto e : enemies_to_remove) reg.kill_entity(e);
    }
    
    bool check_aabb(const position& p1, const collider& c1,
                    const position& p2, const collider& c2) {
        return (p1.x < p2.x + c2.w && p1.x + c1.w > p2.x &&
                p1.y < p2.y + c2.h && p1.y + c1.h > p2.y);
    }
};

extern "C" {
    ISystem* create_system() { return new CollisionSystem(); }
    void destroy_system(ISystem* sys) { delete sys; }
}
```

### Add to CMakeLists.txt
```cmake
add_library(collision_system SHARED Entity/Systems/CollisionSystem.cpp)
target_link_libraries(collision_system PRIVATE ECS)
```

### Load in InGame State
```cpp
// In InGameState::setup_ecs()
loader_.load_system("./build/libcollision_system.so", ILoader::LogicSystem);
```

**For better performance with many entities**, use `PhysicsManager`:  
See [`../ECS/FEATURES.md#feature-3-physics--collision`](../ECS/FEATURES.md)

---

## Step 7: Polish Your Game (20 minutes)

### Add Audio
Using AudioManager from ECS. See [`../ECS/FEATURES.md#feature-4-audio-system`](../ECS/FEATURES.md).

```cpp
// In InGameState::enter()
auto& audio = AudioManager::instance();
audio.init();

auto& music = audio.get_music();
music.load("bgm", "Assets/music.ogg");
music.play("bgm", true);  // Loop

auto& sfx = audio.get_sfx();
sfx.load("shoot", "Assets/shoot.wav");
sfx.load("explosion", "Assets/explosion.wav");

// In PlayerSystem when shooting
AudioManager::instance().get_sfx().play("shoot", 0.7f);

// In CollisionSystem when enemy dies
AudioManager::instance().get_sfx().play("explosion", 0.8f);
```

### Add Particle Effects (Optional)
Create an explosion animation when enemies die:

```cpp
// In CollisionSystem when enemy health <= 0
entity explosion = reg.spawn_entity();
reg.emplace_component<position>(explosion, e_pos.x, e_pos.y);
reg.emplace_component<animation>(explosion, 
    "Assets/explosion.png", 64.0f, 64.0f, 1.0f, 1.0f, 8, false);
// Animation system will remove it when done
```

### Add Menu State (Optional)
Create a main menu before gameplay:

```cpp
class MenuState : public IGameState {
private:
    std::shared_ptr<UI::UIButton> play_button_;

public:
    void enter() override {
        auto screen = RenderManager::instance().get_screen_infos();
        
        play_button_ = std::make_shared<UI::UIButton>(
            "PLAY", screen.getWidth()/2 - 100, screen.getHeight()/2, 200, 50
        );
        play_button_->set_callback([this]() {
            // Transition to InGame
        });
    }
    
    void update(float dt) override {
        play_button_->update(dt);
        
        if (play_button_->is_clicked()) {
            // Switch to InGame state
        }
    }
};
```

See [`RType/Core/States/MainMenu/`](RType/Core/States/MainMenu/) for full menu implementation.

### Improve Visuals
- **Background parallax**: Multiple scrolling layers
- **Better sprites**: Replace placeholder PNGs with proper art
- **Screen shake**: When player is hit
- **Trail effects**: Behind bullets

---

## Advanced Features

### Feature 1: Enemy AI Patterns
Different enemy types with unique behaviors:

```cpp
// In EnemySystem
void update_enemy_ai(Enemy& enemy, position& pos, velocity& vel, float dt) {
    switch (enemy.type) {
        case 0:  // Straight down
            vel.vy = 150.0f;
            break;
        case 1:  // Zigzag
            vel.vx = std::sin(GetTime() * 2.0f) * 100.0f;
            vel.vy = 150.0f;
            break;
        case 2:  // Circular
            float angle = GetTime() * 3.0f;
            vel.vx = std::cos(angle) * 200.0f;
            vel.vy = std::sin(angle) * 200.0f + 100.0f;
            break;
    }
}
```

### Feature 2: Power-Ups
Collectible items that enhance player:

```cpp
struct PowerUp : public IComponent {
    enum Type { SPEED, WEAPON, SHIELD } type;
    PowerUp(Type t) : type(t) {}
};

// Spawn power-up when enemy dies (10% chance)
if (rand() % 10 == 0) {
    entity powerup = reg.spawn_entity();
    reg.emplace_component<position>(powerup, e_pos.x, e_pos.y);
    reg.emplace_component<PowerUp>(powerup, PowerUp::WEAPON);
    // ... sprite, collider
}
```

### Feature 3: Wave System
Progressive difficulty:

```cpp
struct WaveManager : public IComponent {
    int current_wave{1};
    int enemies_per_wave{5};
    int enemies_spawned{0};
    float spawn_rate{2.0f};
    
    void next_wave() {
        current_wave++;
        enemies_per_wave += 3;
        spawn_rate *= 0.9f;  // Spawn faster
        enemies_spawned = 0;
    }
};
```

### Feature 4: High Score System
Save best scores to file:

```cpp
void save_high_score(int score) {
    std::ofstream file("highscore.txt");
    file << score;
}

int load_high_score() {
    std::ifstream file("highscore.txt");
    int score = 0;
    file >> score;
    return score;
}
```

### Feature 5: Multiplayer (Network)
For multiplayer, see:
- [`RType/Core/Client/Network/`](RType/Core/Client/Network/) - Client networking
- [`RType/Core/Server/`](RType/Core/Server/) - Server implementation
- [`RType/Core/Server/Protocol/README.md`](RType/Core/Server/Protocol/README.md) - Protocol documentation

---

## Common Patterns & Best Practices

### Pattern 1: Entity Factory
Centralize entity creation:

```cpp
class EntityFactory {
public:
    static entity create_player(registry& reg, IComponentFactory* factory, 
                                float x, float y) {
        entity player = reg.spawn_entity();
        factory->create_component<position>(reg, player, x, y);
        factory->create_component<velocity>(reg, player, 0.0f, 0.0f);
        factory->create_component<Player>(reg, player, 300.0f, 3, 0);
        factory->create_component<sprite>(reg, player, "Assets/player.png", 64.0f, 64.0f);
        factory->create_component<collider>(reg, player, 64.0f, 64.0f, -32.0f, -32.0f);
        return player;
    }
    
    static entity create_enemy(registry& reg, IComponentFactory* factory,
                               float x, float y, int type) {
        entity enemy = reg.spawn_entity();
        factory->create_component<position>(reg, enemy, x, y);
        factory->create_component<velocity>(reg, enemy, 0.0f, 150.0f);
        factory->create_component<Enemy>(reg, enemy, type, 100.0f, 10);
        factory->create_component<sprite>(reg, enemy, "Assets/enemy.png", 64.0f, 64.0f);
        factory->create_component<collider>(reg, enemy, 64.0f, 64.0f);
        return enemy;
    }
};
```

### Pattern 2: Event-Driven Communication
Use MessagingManager for decoupled systems. See [`../ECS/FEATURES.md#feature-5-messaging-system`](../ECS/FEATURES.md).

```cpp
// System A: Emit event
Event enemy_died("ENEMY_DIED");
enemy_died.set("position_x", enemy_pos.x);
enemy_died.set("position_y", enemy_pos.y);
enemy_died.set("score", enemy.score_value);
MessagingManager::instance().get_event_bus().emit(enemy_died);

// System B: Listen
MessagingManager::instance().get_event_bus().subscribe("ENEMY_DIED", 
    [](const Event& e) {
        float x = e.get<float>("position_x");
        float y = e.get<float>("position_y");
        // Spawn explosion effect at (x, y)
    });
```

### Pattern 3: Object Pooling
Reuse entities instead of spawning/destroying:

```cpp
class BulletPool {
private:
    std::vector<entity> inactive_bullets_;
    
public:
    entity get_bullet(registry& reg, IComponentFactory* factory) {
        if (inactive_bullets_.empty()) {
            return create_new_bullet(reg, factory);
        } else {
            entity bullet = inactive_bullets_.back();
            inactive_bullets_.pop_back();
            // Reset bullet components
            return bullet;
        }
    }
    
    void return_bullet(entity bullet) {
        // Disable bullet components
        inactive_bullets_.push_back(bullet);
    }
};
```

### Pattern 4: Screen Scaling
Make game resolution-independent:

```cpp
// In Constants.hpp
#define BASE_WIDTH 1920.0f
#define BASE_HEIGHT 1080.0f

#define SCALE_X(value) \
    (value * (RenderManager::instance().get_screen_infos().getWidth() / BASE_WIDTH))

#define SCALE_Y(value) \
    (value * (RenderManager::instance().get_screen_infos().getHeight() / BASE_HEIGHT))

// Usage:
float player_speed = SCALE_X(300.0f);  // Scales with resolution
```

See [`RType/Constants.hpp`](RType/Constants.hpp) for complete scaling system.

---

## Best Practices

### Component Design
- **Keep components data-only**: No logic in components (except constructors/getters)
- **Use plain types**: Prefer `float`, `int`, `std::string` over complex objects
- **Minimize dependencies**: Components shouldn't reference other components
- **Name clearly**: `velocity` not `vel`, `Player` not `P`

**Good Component:**
```cpp
struct velocity {
    float x, y;
};
```

**Bad Component:**
```cpp
struct velocity {
    float x, y;
    void apply_to(position& pos, float dt) {  // ❌ Logic in component
        pos.x += x * dt;
        pos.y += y * dt;
    }
};
```

### System Design
- **Single responsibility**: Each system does ONE thing well
- **Iterate components, not entities**: Use registry zippers
- **Avoid frame order dependencies**: Systems should be independent when possible
- **Check entity validity**: Use `reg.has_component<T>(entity)` before access

**System Ordering Tips:**
```cpp
// Run in this order:
// 1. Input systems (PlayerInputSystem)
// 2. Logic systems (AISystem, BulletSystem)
// 3. Physics systems (MovementSystem, CollisionSystem)
// 4. Cleanup systems (CleanupSystem)
// 5. Render systems (RenderSystem)
```

### Performance Tips
- **Use sparse arrays**: Registry already optimizes component storage
- **Batch operations**: Load all assets at once, not per-frame
- **Profile first**: Don't optimize without measuring
- **Use spatial hashing**: For collision detection with many entities (see [`../ECS/FEATURES.md#feature-3-physics--collision`](../ECS/FEATURES.md))

**Performance Comparison:**
```cpp
// ❌ Slow: Check all vs all (O(n²))
for (auto [e1, c1] : reg.view<collider>()) {
    for (auto [e2, c2] : reg.view<collider>()) {
        if (check_collision(c1, c2)) { /* ... */ }
    }
}

// ✅ Fast: Use PhysicsManager (O(n) with spatial hash)
PhysicsManager::instance().check_collisions(reg, [](entity a, entity b) {
    // Handle collision
});
```

### Code Organization
```
MyGame/
├── CMakeLists.txt              # Build configuration
├── Assets/                     # Game assets
│   ├── Sprites/
│   ├── Audio/
│   └── Fonts/
├── Entity/                     # Components
│   ├── Player.hpp
│   ├── Enemy.hpp
│   └── Projectile.hpp
├── Core/
│   ├── Systems/                # Systems (as .cpp for dynamic loading)
│   │   ├── PlayerSystem.cpp
│   │   ├── EnemySystem.cpp
│   │   └── CollisionSystem.cpp
│   └── States/                 # Game states
│       ├── IGameState.hpp
│       ├── Menu.hpp/cpp
│       └── InGame.hpp/cpp
├── Utilities/                  # Helpers
│   ├── EntityFactory.hpp
│   └── Constants.hpp
└── main.cpp                    # Entry point
```

---

## Debugging Tips

### Common Issues

| Problem | Solution |
|---------|----------|
| System not running | Check `extern "C"` export and `reg.add_system()` call |
| Component not found | Verify `factory->create_component<T>()` was called |
| Crash on entity access | Use `reg.has_component<T>(entity)` before accessing |
| Assets not loading | Check file paths are relative to executable location |
| Low FPS | Profile with `SetTargetFPS(60)` and check collision algorithms |
| Memory leak | Ensure `reg.kill_entity()` is called for unused entities |

### Debug Logging Pattern
```cpp
// In system update:
#ifdef DEBUG_MODE
    std::cout << "[PlayerSystem] Active players: " 
              << reg.view<Player>().size() << "\n";
#endif
```

Compile with debug symbols:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

### Entity Inspector
Add a debug system to visualize entities:

```cpp
class DebugSystem : public ISystem {
public:
    void update(registry& reg, IComponentFactory* factory, float dt) override {
        if (IsKeyPressed(KEY_F3)) {
            std::cout << "=== Entity Inspector ===\n";
            for (auto [e, pos] : reg.view<position>().each()) {
                std::cout << "Entity " << e << " at (" << pos.x << ", " << pos.y << ")\n";
                if (reg.has_component<Player>(e)) std::cout << "  [Player]\n";
                if (reg.has_component<Enemy>(e)) std::cout << "  [Enemy]\n";
                if (reg.has_component<Bullet>(e)) std::cout << "  [Bullet]\n";
            }
        }
    }
};
```

---

## Reference Projects

### Complexity Comparison

| Feature | Simple (Pong) | Medium (Space Shooter) | Full (R-Type) |
|---------|---------------|------------------------|---------------|
| **Components** | 4 (position, velocity, sprite, paddle) | 8-10 | 15+ |
| **Systems** | 3 (input, movement, collision) | 6-8 | 12+ |
| **States** | 2 (menu, game) | 3-4 | 5+ (connection, lobby, game, pause, game over) |
| **Lines of Code** | ~500 | ~1500 | ~5000+ |
| **Development Time** | 2-4 hours | 1-2 days | 1-2 weeks |
| **Multiplayer** | Local only | None | Network (UDP) |
| **Assets** | Minimal (rectangles) | Sprites, audio | Full sprite sheets, animations, SFX |

### Example Projects

1. **Pong** (`Games/Pang/`): Simple physics-based game
   - Best for: Learning ECS basics
   - Features: Paddle movement, ball physics, scoring

2. **Space Shooter** (This tutorial): Medium complexity
   - Best for: Understanding system interactions
   - Features: Player, enemies, bullets, collisions, score

3. **R-Type** (`Games/RType/`): Full multiplayer game
   - Best for: Production reference
   - Features: Networking, state machine, lobby system, advanced UI
   - See: [`RType/README.md`](RType/README.md), [`RType/Networking.md`](RType/Networking.md)

---

## Quick Start Checklist

**For New Game Prototype:**
- [ ] Copy `MyGame/` template structure
- [ ] Update `CMakeLists.txt` with game name
- [ ] Define components in `Entity/*.hpp`
- [ ] Create systems in `Core/Systems/*.cpp` with `extern "C"` exports
- [ ] Implement `IGameState` for each state
- [ ] Create `main.cpp` with game loop
- [ ] Load systems with `PlatformLoader`
- [ ] Add assets to `Assets/` directory
- [ ] Compile with `cmake .. && make`
- [ ] Test and iterate

**For Adding Feature:**
- [ ] Identify required components (data)
- [ ] Create system that processes those components (logic)
- [ ] Compile system as shared library
- [ ] Load in game state with `reg.add_system()`
- [ ] Test with entity inspector (`F3` key)

**For Debugging:**
- [ ] Check console for system load errors
- [ ] Verify component registration with `has_component<T>()`
- [ ] Add debug logging in systems
- [ ] Use `DEBUG_MODE` preprocessor flag
- [ ] Profile with frame time monitoring

---

## Additional Resources

### Documentation
- **ECS Engine**: [`../ECS/README.md`](../ECS/README.md) - Core architecture
- **ECS Features**: [`../ECS/FEATURES.md`](../ECS/FEATURES.md) - Deep-dive on managers and systems
- **R-Type Networking**: [`RType/Networking.md`](RType/Networking.md) - Multiplayer implementation

### Code Examples
- **Pang Game**: `Games/Pang/` - Simple physics game
- **R-Type**: `Games/RType/` - Full multiplayer implementation
- **ECS Tests**: `ECS/test/` - Unit tests showing API usage

### External References
- [Raylib Cheatsheet](https://www.raylib.com/cheatsheet/cheatsheet.html) - Rendering API
- [EnTT Documentation](https://github.com/skypjack/entt/wiki) - Registry inspiration
- [Game Programming Patterns](https://gameprogrammingpatterns.com/) - Design patterns

---

## Contributing

Found an issue or want to improve the documentation? 

1. Check existing examples in `Games/RType/` and `Games/Pang/`
2. Test your changes by creating a new prototype following this guide
3. Ensure code examples compile and run
4. Update both code and documentation together

---

## License

This ECS engine and documentation are part of the R-Type project.  
See root `README.md` for license information.

---

**Happy Game Development! 🎮**

*Estimated completion time for this tutorial: 2-3 hours*  
*Questions? Check [`../ECS/README.md`](../ECS/README.md) or reference the R-Type implementation.*

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

See `RType/Core/Services/Network/` and `RType/Core/Server/Protocol/README.md` for reference.

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
- **Network Protocol**: `RType/Core/Server/Protocol/README.md` (if using networking)
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
