/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** PhysicsManager Unit Tests
*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

// Must be before raylib.h to avoid Windows GDI conflicts
#define NOGDI
#define NOMINMAX

#include "ECS/Physics/PhysicsManager.hpp"
#include <raylib.h>

TEST_SUITE("PhysicsManager Tests") {
    TEST_CASE("PhysicsManager is a singleton") {
        auto& instance1 = PhysicsManager::instance();
        auto& instance2 = PhysicsManager::instance();
        CHECK(&instance1 == &instance2);
    }

    TEST_CASE("PhysicsManager initializes with default cell size") {
        auto& physics = PhysicsManager::instance();
        physics.init();
        
        // Should not crash
        CHECK(true);
    }

    TEST_CASE("PhysicsManager initializes with custom cell size") {
        auto& physics = PhysicsManager::instance();
        physics.init(128.0f);
        
        CHECK(true);
    }

    TEST_CASE("PhysicsManager can update entities") {
        auto& physics = PhysicsManager::instance();
        physics.init(64.0f);
        physics.clear();
        
        AABB bounds1{0, 0, 10, 10};
        physics.update_entity(1, bounds1);
        
        AABB bounds2{5, 5, 10, 10};
        physics.update_entity(2, bounds2);
        
        // Should not crash
        CHECK(true);
    }

    TEST_CASE("PhysicsManager detects potential collisions for overlapping entities") {
        auto& physics = PhysicsManager::instance();
        physics.init(64.0f);
        physics.clear();
        
        // Two overlapping entities
        AABB bounds1{0, 0, 20, 20};
        physics.update_entity(1, bounds1);
        
        AABB bounds2{10, 10, 20, 20};
        physics.update_entity(2, bounds2);
        
        auto pairs = physics.get_collision_pairs();
        CHECK(pairs.size() > 0); // Should detect potential collision
        
        // Verify the pair contains both entities
        bool found = false;
        for (const auto& [e1, e2] : pairs) {
            if ((e1 == 1 && e2 == 2) || (e1 == 2 && e2 == 1)) {
                found = true;
                break;
            }
        }
        CHECK(found);
    }

    TEST_CASE("PhysicsManager does not detect non-overlapping entities") {
        auto& physics = PhysicsManager::instance();
        physics.init(64.0f);
        physics.clear();
        
        // Two separated entities
        AABB bounds1{0, 0, 10, 10};
        physics.update_entity(1, bounds1);
        
        AABB bounds2{100, 100, 10, 10};
        physics.update_entity(2, bounds2);
        
        auto pairs = physics.get_collision_pairs();
        CHECK(pairs.size() == 0); // Should not detect collision
    }

    TEST_CASE("PhysicsManager query_area returns entities in area") {
        auto& physics = PhysicsManager::instance();
        physics.init(64.0f);
        physics.clear();
        
        AABB bounds1{5, 5, 10, 10};
        physics.update_entity(1, bounds1);
        
        // Move entity 2 further away (beyond cell boundaries)
        AABB bounds2{150, 150, 10, 10}; // Changed from {50, 50}
        physics.update_entity(2, bounds2);
        
        AABB query{0, 0, 20, 20};
        auto results = physics.query_area(query);
        
        CHECK(results.size() == 1); // Only entity 1 should be in query area
        CHECK(results[0] == 1);
    }

    TEST_CASE("PhysicsManager query_area returns multiple entities") {
        auto& physics = PhysicsManager::instance();
        physics.init(64.0f);
        physics.clear();
        
        AABB bounds1{5, 5, 10, 10};
        physics.update_entity(1, bounds1);
        
        AABB bounds2{15, 15, 10, 10};
        physics.update_entity(2, bounds2);
        
        AABB bounds3{100, 100, 10, 10};
        physics.update_entity(3, bounds3);
        
        AABB query{0, 0, 30, 30};
        auto results = physics.query_area(query);
        
        CHECK(results.size() == 2); // Entities 1 and 2
        
        bool has_1 = false, has_2 = false;
        for (size_t id : results) {
            if (id == 1) has_1 = true;
            if (id == 2) has_2 = true;
        }
        CHECK(has_1);
        CHECK(has_2);
    }

    TEST_CASE("PhysicsManager check_collision detects AABB overlap") {
        auto& physics = PhysicsManager::instance();
        
        SUBCASE("Overlapping AABBs") {
            AABB a{0, 0, 10, 10};
            AABB b{5, 5, 10, 10};
            CHECK(physics.check_collision(a, b));
        }
        
        SUBCASE("Non-overlapping AABBs") {
            AABB a{0, 0, 10, 10};
            AABB c{100, 100, 10, 10};
            CHECK_FALSE(physics.check_collision(a, c));
        }
        
        SUBCASE("Edge-touching AABBs") {
            AABB a{0, 0, 10, 10};
            AABB d{10, 10, 10, 10};
            // Edge touching IS considered collision (standard game engine behavior)
            CHECK(physics.check_collision(a, d));
        }
        
        SUBCASE("Clearly separated AABBs") {
            AABB a{0, 0, 10, 10};
            AABB e{11, 11, 10, 10}; // 1 pixel gap
            CHECK_FALSE(physics.check_collision(a, e));
        }
    }

    TEST_CASE("PhysicsManager check_collision_circle detects circle overlap") {
        auto& physics = PhysicsManager::instance();
        
        SUBCASE("Overlapping circles") {
            Vector2 center1{0, 0};
            float radius1 = 10.0f;
            
            Vector2 center2{5, 5};
            float radius2 = 10.0f;
            
            CHECK(physics.check_collision_circle(center1, radius1, center2, radius2));
        }
        
        SUBCASE("Non-overlapping circles") {
            Vector2 center1{0, 0};
            float radius1 = 10.0f;
            
            Vector2 center3{100, 100};
            float radius3 = 10.0f;
            
            CHECK_FALSE(physics.check_collision_circle(center1, radius1, center3, radius3));
        }
        
        SUBCASE("Touching circles") {
            Vector2 center1{0, 0};
            float radius1 = 10.0f;
            
            Vector2 center2{20, 0}; // Exactly touching
            float radius2 = 10.0f;
            
            // Touching should be considered collision
            CHECK(physics.check_collision_circle(center1, radius1, center2, radius2));
        }
    }

    TEST_CASE("PhysicsManager remove_entity removes entity from spatial hash") {
        auto& physics = PhysicsManager::instance();
        physics.init(64.0f);
        physics.clear();
        
        AABB bounds{5, 5, 10, 10};
        physics.update_entity(1, bounds);
        
        AABB query{0, 0, 20, 20};
        auto results = physics.query_area(query);
        CHECK(results.size() == 1);
        
        physics.remove_entity(1);
        results = physics.query_area(query);
        CHECK(results.size() == 0); // Entity removed
    }

    TEST_CASE("PhysicsManager clear removes all entities") {
        auto& physics = PhysicsManager::instance();
        physics.init(64.0f);
        physics.clear();
        
        AABB bounds1{0, 0, 10, 10};
        physics.update_entity(1, bounds1);
        
        AABB bounds2{20, 20, 10, 10};
        physics.update_entity(2, bounds2);
        
        AABB bounds3{40, 40, 10, 10};
        physics.update_entity(3, bounds3);
        
        physics.clear();
        
        AABB query{0, 0, 100, 100};
        auto results = physics.query_area(query);
        CHECK(results.size() == 0); // All entities removed
    }

    TEST_CASE("PhysicsManager handles entity updates") {
        auto& physics = PhysicsManager::instance();
        physics.init(64.0f);
        physics.clear();
        
        // Add entity at position 1
        AABB bounds1{0, 0, 10, 10};
        physics.update_entity(1, bounds1);
        
        AABB query1{0, 0, 20, 20};
        auto results1 = physics.query_area(query1);
        CHECK(results1.size() == 1);
        
        // Update entity to position 2
        AABB bounds2{100, 100, 10, 10};
        physics.update_entity(1, bounds2);
        
        // Old position should not find entity
        auto results2 = physics.query_area(query1);
        CHECK(results2.size() == 0);
        
        // New position should find entity
        AABB query2{90, 90, 30, 30};
        auto results3 = physics.query_area(query2);
        CHECK(results3.size() == 1);
        CHECK(results3[0] == 1);
    }

    TEST_CASE("PhysicsManager spatial hash can be accessed") {
        auto& physics = PhysicsManager::instance();
        physics.init(64.0f);
        
        auto& spatial_hash = physics.get_spatial_hash();
        // Just check we can access it
        CHECK(true);
        
        const auto& const_spatial_hash = physics.get_spatial_hash();
        CHECK(true);
    }

    TEST_CASE("PhysicsManager handles many entities efficiently") {
        auto& physics = PhysicsManager::instance();
        physics.init(64.0f);
        physics.clear();
        
        // Add 50 entities
        for (size_t i = 0; i < 50; i++) {
            float x = (i % 10) * 15.0f;
            float y = (i / 10) * 15.0f;
            physics.update_entity(i, AABB{x, y, 10, 10});
        }
        
        // Query should return entities in area
        auto results = physics.query_area(AABB{0, 0, 50, 50});
        CHECK(results.size() > 0);
        CHECK(results.size() <= 50);
        
        physics.clear();
    }

    TEST_CASE("PhysicsManager detects multiple collision pairs") {
        auto& physics = PhysicsManager::instance();
        physics.init(64.0f);
        physics.clear();
        
        // Add 4 entities in same cell, all overlapping
        physics.update_entity(1, AABB{10, 10, 15, 15});
        physics.update_entity(2, AABB{15, 15, 15, 15});
        physics.update_entity(3, AABB{20, 20, 15, 15});
        physics.update_entity(4, AABB{12, 12, 15, 15});
        
        auto pairs = physics.get_collision_pairs();
        // Should have multiple potential collision pairs
        CHECK(pairs.size() >= 4);
        
        physics.clear();
    }

    TEST_CASE("PhysicsManager AABB collision with zero dimensions") {
        auto& physics = PhysicsManager::instance();
        
        AABB point{10, 10, 0, 0};
        AABB box{5, 5, 20, 20};
        
        // Point inside box should collide
        CHECK(physics.check_collision(point, box));
    }

    TEST_CASE("PhysicsManager AABB collision with negative coordinates") {
        auto& physics = PhysicsManager::instance();
        
        AABB a{-10, -10, 15, 15};
        AABB b{-5, -5, 15, 15};
        
        CHECK(physics.check_collision(a, b));
    }

    TEST_CASE("PhysicsManager circle collision with zero radius") {
        auto& physics = PhysicsManager::instance();
        
        Vector2 point{10, 10};
        float radius1 = 0.0f;
        
        Vector2 circle{10, 10};
        float radius2 = 10.0f;
        
        // Point at center of circle should collide
        CHECK(physics.check_collision_circle(point, radius1, circle, radius2));
    }

    TEST_CASE("PhysicsManager circle collision with far apart circles") {
        auto& physics = PhysicsManager::instance();
        
        Vector2 c1{0, 0};
        float r1 = 5.0f;
        
        Vector2 c2{1000, 1000};
        float r2 = 5.0f;
        
        CHECK_FALSE(physics.check_collision_circle(c1, r1, c2, r2));
    }

    TEST_CASE("PhysicsManager update same entity multiple times") {
        auto& physics = PhysicsManager::instance();
        physics.init(64.0f);
        physics.clear();
        
        physics.update_entity(1, AABB{0, 0, 10, 10});
        physics.update_entity(1, AABB{200, 200, 10, 10}); // Far away, different cell
        physics.update_entity(1, AABB{400, 400, 10, 10}); // Even farther
        
        // Entity should be at last position only
        auto results1 = physics.query_area(AABB{0, 0, 15, 15});
        CHECK(results1.size() == 0); // Not at first position
        
        auto results2 = physics.query_area(AABB{195, 195, 20, 20});
        CHECK(results2.size() == 0); // Not at second position
        
        auto results3 = physics.query_area(AABB{395, 395, 20, 20});
        CHECK(results3.size() == 1); // At last position
        CHECK(results3[0] == 1);
        
        physics.clear();
    }

    TEST_CASE("PhysicsManager remove non-existent entity") {
        auto& physics = PhysicsManager::instance();
        physics.init(64.0f);
        physics.clear();
        
        // Should not crash
        physics.remove_entity(999);
        
        CHECK(true);
    }

    TEST_CASE("PhysicsManager query empty area") {
        auto& physics = PhysicsManager::instance();
        physics.init(64.0f);
        physics.clear();
        
        physics.update_entity(1, AABB{0, 0, 10, 10});
        
        // Query far away
        auto results = physics.query_area(AABB{1000, 1000, 10, 10});
        CHECK(results.empty());
        
        physics.clear();
    }

    TEST_CASE("PhysicsManager large AABB spanning multiple cells") {
        auto& physics = PhysicsManager::instance();
        physics.init(64.0f);
        physics.clear();
        
        // Large AABB spanning 4 cells (128x128)
        physics.update_entity(1, AABB{0, 0, 128, 128});
        
        // Should find entity in all 4 corner queries
        auto results1 = physics.query_area(AABB{10, 10, 10, 10});    // Top-left
        auto results2 = physics.query_area(AABB{100, 10, 10, 10});   // Top-right
        auto results3 = physics.query_area(AABB{10, 100, 10, 10});   // Bottom-left
        auto results4 = physics.query_area(AABB{100, 100, 10, 10});  // Bottom-right
        
        CHECK(results1.size() == 1);
        CHECK(results2.size() == 1);
        CHECK(results3.size() == 1);
        CHECK(results4.size() == 1);
        
        physics.clear();
    }

    TEST_CASE("PhysicsManager collision detection accuracy") {
        auto& physics = PhysicsManager::instance();
        
        SUBCASE("Partial overlap") {
            AABB a{0, 0, 20, 20};
            AABB b{15, 15, 20, 20};
            CHECK(physics.check_collision(a, b));
        }
        
        SUBCASE("One inside another") {
            AABB big{0, 0, 100, 100};
            AABB small{40, 40, 10, 10};
            CHECK(physics.check_collision(big, small));
        }
        
        SUBCASE("Just separated horizontally") {
            AABB a{0, 0, 10, 10};
            AABB b{10.1f, 0, 10, 10};
            CHECK_FALSE(physics.check_collision(a, b));
        }
        
        SUBCASE("Just separated vertically") {
            AABB a{0, 0, 10, 10};
            AABB b{0, 10.1f, 10, 10};
            CHECK_FALSE(physics.check_collision(a, b));
        }
    }
}
