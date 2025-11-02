/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** ECS Core Logic Unit Tests (Registry, Components, Systems, Zipper)
*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "ECS/Registry.hpp"
#include "ECS/Components.hpp"
#include "ECS/Systems/ISystem.hpp"
#include "ECS/Zipper.hpp"
#include <raylib.h>

// Helper function to check if entity has component
template<typename Component>
bool has_component(registry& reg, size_t entity_id) {
    auto* arr = reg.get_if<Component>();
    if (!arr) return false;
    using idx_t = typename sparse_set<Component>::size_type;
    return arr->has(static_cast<idx_t>(entity_id));
}

TEST_SUITE("Registry Tests") {
    TEST_CASE("Registry can spawn entities") {
        registry reg;
        
        entity e1 = reg.spawn_entity();
        entity e2 = reg.spawn_entity();
        entity e3 = reg.spawn_entity();
        
        CHECK(e1 != e2);
        CHECK(e2 != e3);
        CHECK(e1 != e3);
    }

    TEST_CASE("Registry can add and retrieve components") {
        registry reg;
        
        entity e = reg.spawn_entity();
        
        // Add position component
        reg.add_component<position>(e, position{100.0f, 200.0f});
        
        // Retrieve component
        CHECK(has_component<position>(reg, e));
        auto& pos = reg.get_component<position>(e);
        CHECK(pos.x == doctest::Approx(100.0f));
        CHECK(pos.y == doctest::Approx(200.0f));
    }

    TEST_CASE("Registry has_component returns false for missing components") {
        registry reg;
        
        entity e = reg.spawn_entity();
        
        CHECK_FALSE(has_component<position>(reg, e));
        CHECK_FALSE(has_component<velocity>(reg, e));
    }

    TEST_CASE("Registry can remove components") {
        registry reg;
        
        entity e = reg.spawn_entity();
        reg.add_component<position>(e, position{50.0f, 50.0f});
        
        CHECK(has_component<position>(reg, e));
        
        reg.remove_component<position>(e);
        
        CHECK_FALSE(has_component<position>(reg, e));
    }

    TEST_CASE("Registry can kill entities") {
        registry reg;
        
        entity e1 = reg.spawn_entity();
        entity e2 = reg.spawn_entity();
        
        reg.add_component<position>(e1, position{10.0f, 10.0f});
        reg.add_component<position>(e2, position{20.0f, 20.0f});
        
        reg.kill_entity(e1);
        
        // e2 should still exist
        CHECK(has_component<position>(reg, e2));
    }

    TEST_CASE("Registry sparse_set returns all entities with component") {
        registry reg;
        
        entity e1 = reg.spawn_entity();
        entity e2 = reg.spawn_entity();
        entity e3 = reg.spawn_entity();
        
        reg.add_component<position>(e1, position{1.0f, 1.0f});
        reg.add_component<position>(e2, position{2.0f, 2.0f});
        // e3 has no position
        
        auto* positions = reg.get_if<position>();
        CHECK(positions != nullptr);
        CHECK(positions->size() == 2);
    }

    TEST_CASE("Registry get_if returns array of components") {
        registry reg;
        
        entity e1 = reg.spawn_entity();
        entity e2 = reg.spawn_entity();
        
        reg.add_component<velocity>(e1, velocity{10.0f, 20.0f});
        reg.add_component<velocity>(e2, velocity{30.0f, 40.0f});
        
        auto* velocities = reg.get_if<velocity>();
        CHECK(velocities != nullptr);
        CHECK(velocities->size() == 2);
        
        // Check values
        int found = 0;
        for (const auto& vel : *velocities) {
            if (vel.vx == 10.0f || vel.vx == 30.0f) {
                found++;
            }
        }
        CHECK(found >= 1);
    }
}

TEST_SUITE("Component Tests") {
    TEST_CASE("Position component has correct default values") {
        position pos;
        CHECK(pos.x == doctest::Approx(0.0f));
        CHECK(pos.y == doctest::Approx(0.0f));
    }

    TEST_CASE("Position component can be initialized") {
        position pos{100.5f, 200.5f};
        CHECK(pos.x == doctest::Approx(100.5f));
        CHECK(pos.y == doctest::Approx(200.5f));
    }

    TEST_CASE("Velocity component works correctly") {
        velocity vel{50.0f, -30.0f};
        CHECK(vel.vx == doctest::Approx(50.0f));
        CHECK(vel.vy == doctest::Approx(-30.0f));
    }

    TEST_CASE("Sprite component can be created") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        sprite spr;
        spr.texture_path = "test.png";
        spr.width = 64.0f;
        spr.height = 64.0f;
        
        CHECK(spr.texture_path == "test.png");
        CHECK(spr.width == doctest::Approx(64.0f));
        CHECK(spr.height == doctest::Approx(64.0f));
        CHECK(spr.visible == true);
        
        CloseWindow();
    }

    TEST_CASE("Collider component has correct properties") {
        collider col{32.0f, 32.0f, -16.0f, -16.0f};
        
        CHECK(col.w == doctest::Approx(32.0f));
        CHECK(col.h == doctest::Approx(32.0f));
        CHECK(col.offset_x == doctest::Approx(-16.0f));
        CHECK(col.offset_y == doctest::Approx(-16.0f));
    }
}

TEST_SUITE("Zipper Tests") {
    TEST_CASE("zipper iterates over entities with multiple components") {
        registry reg;
        
        entity e1 = reg.spawn_entity();
        entity e2 = reg.spawn_entity();
        entity e3 = reg.spawn_entity();
        
        // e1: position + velocity
        reg.add_component<position>(e1, position{10.0f, 10.0f});
        reg.add_component<velocity>(e1, velocity{1.0f, 1.0f});
        
        // e2: only position
        reg.add_component<position>(e2, position{20.0f, 20.0f});
        
        // e3: position + velocity
        reg.add_component<position>(e3, position{30.0f, 30.0f});
        reg.add_component<velocity>(e3, velocity{2.0f, 2.0f});
        
        auto* positions = reg.get_if<position>();
        auto* velocities = reg.get_if<velocity>();
        
        int count = 0;
        for (auto [pos, vel, entity_id] : zipper(*positions, *velocities)) {
            count++;
            CHECK(has_component<position>(reg, entity_id));
            CHECK(has_component<velocity>(reg, entity_id));
        }
        
        CHECK(count == 2); // Only e1 and e3 have both components
    }

    TEST_CASE("zipper allows modification of components") {
        registry reg;
        
        entity e = reg.spawn_entity();
        reg.add_component<position>(e, position{0.0f, 0.0f});
        reg.add_component<velocity>(e, velocity{5.0f, 10.0f});
        
        auto* positions = reg.get_if<position>();
        auto* velocities = reg.get_if<velocity>();
        
        // Modify via zipper
        for (auto [pos, vel, entity] : zipper(*positions, *velocities)) {
            pos.x += vel.vx;
            pos.y += vel.vy;
        }
        
        auto& pos = reg.get_component<position>(e);
        CHECK(pos.x == doctest::Approx(5.0f));
        CHECK(pos.y == doctest::Approx(10.0f));
    }

    TEST_CASE("zipper works with three components") {
        registry reg;
        
        entity e1 = reg.spawn_entity();
        reg.add_component<position>(e1, position{1.0f, 1.0f});
        reg.add_component<velocity>(e1, velocity{2.0f, 2.0f});
        reg.add_component<sprite>(e1, sprite{});
        
        entity e2 = reg.spawn_entity();
        reg.add_component<position>(e2, position{3.0f, 3.0f});
        reg.add_component<velocity>(e2, velocity{4.0f, 4.0f});
        // e2 missing sprite
        
        auto* positions = reg.get_if<position>();
        auto* velocities = reg.get_if<velocity>();
        auto* sprites = reg.get_if<sprite>();
        
        int count = 0;
        for (auto [pos, vel, spr, e] : zipper(*positions, *velocities, *sprites)) {
            count++;
            CHECK(has_component<position>(reg, e));
            CHECK(has_component<velocity>(reg, e));
            CHECK(has_component<sprite>(reg, e));
        }
        
        CHECK(count == 1); // Only e1 has all three
    }
}

TEST_SUITE("System Tests") {
    // Simple test system
    class TestSystem : public ISystem {
    public:
        int update_count = 0;
        
        void update(registry& r, float dt) override {
            update_count++;
        }
        
        const char* get_name() const override {
            return "TestSystem";
        }
    };

    TEST_CASE("System can be updated") {
        registry reg;
        TestSystem system;
        
        CHECK(system.update_count == 0);
        
        system.update(reg, 0.016f);
        CHECK(system.update_count == 1);
        
        system.update(reg, 0.016f);
        CHECK(system.update_count == 2);
    }

    TEST_CASE("System get_name returns correct name") {
        TestSystem system;
        CHECK(std::string(system.get_name()) == "TestSystem");
    }

    TEST_CASE("System can access registry") {
        class RegistryTestSystem : public ISystem {
        public:
            int entity_count = 0;
            
            void update(registry& r, float dt) override {
                entity_count = 0;
                auto* positions = r.get_if<position>();
                if (positions) {
                    entity_count = positions->size();
                }
            }
            
            const char* get_name() const override {
                return "RegistryTestSystem";
            }
        };
        
        registry reg;
        RegistryTestSystem system;
        
        entity e1 = reg.spawn_entity();
        entity e2 = reg.spawn_entity();
        reg.add_component<position>(e1, position{1.0f, 1.0f});
        reg.add_component<position>(e2, position{2.0f, 2.0f});
        
        system.update(reg, 0.016f);
        CHECK(system.entity_count == 2);
    }

    TEST_CASE("System can modify components") {
        class MovementSystem : public ISystem {
        public:
            void update(registry& r, float dt) override {
                auto* positions = r.get_if<position>();
                auto* velocities = r.get_if<velocity>();
                if (!positions || !velocities) return;
                
                for (auto [pos, vel, e] : zipper(*positions, *velocities)) {
                    pos.x += vel.vx * dt;
                    pos.y += vel.vy * dt;
                }
            }
            
            const char* get_name() const override {
                return "MovementSystem";
            }
        };
        
        registry reg;
        MovementSystem system;
        
        entity e = reg.spawn_entity();
        reg.add_component<position>(e, position{0.0f, 0.0f});
        reg.add_component<velocity>(e, velocity{100.0f, 200.0f});
        
        system.update(reg, 0.1f); // 0.1 second
        
        auto& pos = reg.get_component<position>(e);
        CHECK(pos.x == doctest::Approx(10.0f));
        CHECK(pos.y == doctest::Approx(20.0f));
    }
}

TEST_SUITE("Integration Tests") {
    TEST_CASE("Complete ECS workflow") {
        registry reg;
        
        // Create entities
        entity player = reg.spawn_entity();
        entity enemy = reg.spawn_entity();
        
        // Add components
        reg.add_component<position>(player, position{0.0f, 0.0f});
        reg.add_component<velocity>(player, velocity{50.0f, 0.0f});
        
        reg.add_component<position>(enemy, position{100.0f, 0.0f});
        reg.add_component<velocity>(enemy, velocity{-25.0f, 0.0f});
        
        auto* positions = reg.get_if<position>();
        auto* velocities = reg.get_if<velocity>();
        
        // Simulate movement
        float dt = 0.1f;
        for (auto [pos, vel, e] : zipper(*positions, *velocities)) {
            pos.x += vel.vx * dt;
            pos.y += vel.vy * dt;
        }
        
        // Check results
        auto& player_pos = reg.get_component<position>(player);
        auto& enemy_pos = reg.get_component<position>(enemy);
        
        CHECK(player_pos.x == doctest::Approx(5.0f));
        CHECK(enemy_pos.x == doctest::Approx(97.5f));
    }

    TEST_CASE("Entity lifecycle management") {
        registry reg;
        
        // Spawn multiple entities
        std::vector<entity> entities;
        for (int i = 0; i < 10; i++) {
            entity e = reg.spawn_entity();
            reg.add_component<position>(e, position{float(i), float(i)});
            entities.push_back(e);
        }
        
        // Verify all exist
        auto* positions = reg.get_if<position>();
        CHECK(positions->size() == 10);
        
        // Kill half
        for (int i = 0; i < 5; i++) {
            reg.kill_entity(entities[i]);
        }
        
        // Verify half remain
        positions = reg.get_if<position>();
        CHECK(positions->size() == 5);
    }

    TEST_CASE("Multiple systems processing same entities") {
        class System1 : public ISystem {
        public:
            void update(registry& r, float dt) override {
                auto* velocities = r.get_if<velocity>();
                if (!velocities) return;
                for (auto& vel : *velocities) {
                    vel.vx *= 2.0f;
                }
            }
            const char* get_name() const override { return "System1"; }
        };
        
        class System2 : public ISystem {
        public:
            void update(registry& r, float dt) override {
                auto* velocities = r.get_if<velocity>();
                if (!velocities) return;
                for (auto& vel : *velocities) {
                    vel.vy *= 3.0f;
                }
            }
            const char* get_name() const override { return "System2"; }
        };
        
        registry reg;
        System1 sys1;
        System2 sys2;
        
        entity e = reg.spawn_entity();
        reg.add_component<velocity>(e, velocity{10.0f, 20.0f});
        
        sys1.update(reg, 0.0f);
        sys2.update(reg, 0.0f);
        
        auto& vel = reg.get_component<velocity>(e);
        CHECK(vel.vx == doctest::Approx(20.0f)); // 10 * 2
        CHECK(vel.vy == doctest::Approx(60.0f)); // 20 * 3
    }
}
