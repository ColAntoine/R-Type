#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "ECS/Physics/SpatialHash/SpatialHash.hpp"

TEST_SUITE("SpatialHash Tests") {
    TEST_CASE("SpatialHash initializes with default cell size") {
        SpatialHash hash;
        CHECK(hash.get_cell_size() == 64.0f);
        CHECK(hash.get_entity_count() == 0);
    }

    TEST_CASE("SpatialHash initializes with custom cell size") {
        SpatialHash hash(128.0f);
        CHECK(hash.get_cell_size() == 128.0f);
        CHECK(hash.get_entity_count() == 0);
    }

    TEST_CASE("SpatialHash can change cell size") {
        SpatialHash hash;
        hash.set_cell_size(32.0f);
        CHECK(hash.get_cell_size() == 32.0f);
    }

    TEST_CASE("SpatialHash inserts entity correctly") {
        SpatialHash hash(64.0f);
        AABB box{0, 0, 10, 10};
        
        hash.insert(1, box);
        CHECK(hash.get_entity_count() == 1);
    }

    TEST_CASE("SpatialHash inserts multiple entities") {
        SpatialHash hash(64.0f);
        
        hash.insert(1, AABB{0, 0, 10, 10});
        hash.insert(2, AABB{20, 20, 10, 10});
        hash.insert(3, AABB{40, 40, 10, 10});
        
        CHECK(hash.get_entity_count() == 3);
    }

    TEST_CASE("SpatialHash removes entity correctly") {
        SpatialHash hash(64.0f);
        AABB box{0, 0, 10, 10};
        
        hash.insert(1, box);
        CHECK(hash.get_entity_count() == 1);
        
        hash.remove(1);
        CHECK(hash.get_entity_count() == 0);
    }

    TEST_CASE("SpatialHash removes only specified entity") {
        SpatialHash hash(64.0f);
        
        hash.insert(1, AABB{0, 0, 10, 10});
        hash.insert(2, AABB{20, 20, 10, 10});
        
        hash.remove(1);
        CHECK(hash.get_entity_count() == 1);
    }

    TEST_CASE("SpatialHash removes non-existent entity gracefully") {
        SpatialHash hash(64.0f);
        
        hash.insert(1, AABB{0, 0, 10, 10});
        hash.remove(999); // Non-existent
        
        CHECK(hash.get_entity_count() == 1);
    }

    TEST_CASE("SpatialHash clears all entities") {
        SpatialHash hash(64.0f);
        
        hash.insert(1, AABB{0, 0, 10, 10});
        hash.insert(2, AABB{20, 20, 10, 10});
        hash.insert(3, AABB{40, 40, 10, 10});
        
        hash.clear();
        CHECK(hash.get_entity_count() == 0);
    }

    TEST_CASE("SpatialHash query finds entity in same cell") {
        SpatialHash hash(64.0f);
        
        // Both entities in cell (0, 0): positions 0-63
        hash.insert(1, AABB{10, 10, 10, 10});
        hash.insert(2, AABB{30, 30, 10, 10});
        
        auto results = hash.query(AABB{10, 10, 10, 10});
        CHECK(results.size() >= 1); // At least finds itself
    }

    TEST_CASE("SpatialHash query does not find entity in different cell") {
        SpatialHash hash(64.0f);
        
        // Entity 1 in cell (0, 0): [0, 64)
        hash.insert(1, AABB{10, 10, 10, 10});
        
        // Entity 2 in cell (2, 2): [128, 192)
        hash.insert(2, AABB{150, 150, 10, 10});
        
        // Query in cell (0, 0)
        auto results = hash.query(AABB{10, 10, 10, 10});
        
        // Should only find entity 1, not entity 2
        bool found_1 = false;
        bool found_2 = false;
        for (auto id : results) {
            if (id == 1) found_1 = true;
            if (id == 2) found_2 = true;
        }
        
        CHECK(found_1 == true);
        CHECK(found_2 == false);
    }

    TEST_CASE("SpatialHash query with empty hash returns empty") {
        SpatialHash hash(64.0f);
        
        auto results = hash.query(AABB{0, 0, 10, 10});
        CHECK(results.empty());
    }

    TEST_CASE("SpatialHash handles entity at cell boundary") {
        SpatialHash hash(64.0f);
        
        // Entity exactly at cell boundary (64, 64)
        hash.insert(1, AABB{64, 64, 10, 10});
        
        auto results = hash.query(AABB{64, 64, 10, 10});
        CHECK(results.size() >= 1);
    }

    TEST_CASE("SpatialHash handles large entity spanning multiple cells") {
        SpatialHash hash(64.0f);
        
        // Large entity spanning cells (0,0), (1,0), (0,1), (1,1)
        hash.insert(1, AABB{30, 30, 80, 80});
        
        // Query in cell (0, 0)
        auto results1 = hash.query(AABB{10, 10, 10, 10});
        
        // Query in cell (1, 0)
        auto results2 = hash.query(AABB{70, 10, 10, 10});
        
        // Entity 1 should appear in both queries
        bool found_in_1 = std::find(results1.begin(), results1.end(), 1) != results1.end();
        bool found_in_2 = std::find(results2.begin(), results2.end(), 1) != results2.end();
        
        CHECK(found_in_1);
        CHECK(found_in_2);
    }

    TEST_CASE("SpatialHash handles entity movement across cells") {
        SpatialHash hash(64.0f);
        
        // Insert in cell (0, 0)
        hash.insert(1, AABB{10, 10, 10, 10});
        
        auto results1 = hash.query(AABB{10, 10, 10, 10});
        CHECK(std::find(results1.begin(), results1.end(), 1) != results1.end());
        
        // Remove and reinsert in cell (2, 2)
        hash.remove(1);
        hash.insert(1, AABB{150, 150, 10, 10});
        
        // Should not find in old cell
        auto results2 = hash.query(AABB{10, 10, 10, 10});
        CHECK(std::find(results2.begin(), results2.end(), 1) == results2.end());
        
        // Should find in new cell
        auto results3 = hash.query(AABB{150, 150, 10, 10});
        CHECK(std::find(results3.begin(), results3.end(), 1) != results3.end());
    }

    TEST_CASE("SpatialHash get_potential_collisions with no entities") {
        SpatialHash hash(64.0f);
        
        auto collisions = hash.get_potential_collisions();
        CHECK(collisions.empty());
    }

    TEST_CASE("SpatialHash get_potential_collisions with single entity") {
        SpatialHash hash(64.0f);
        
        hash.insert(1, AABB{10, 10, 10, 10});
        
        auto collisions = hash.get_potential_collisions();
        CHECK(collisions.empty()); // No pairs with single entity
    }

    TEST_CASE("SpatialHash get_potential_collisions with entities in same cell") {
        SpatialHash hash(64.0f);
        
        // Both in cell (0, 0)
        hash.insert(1, AABB{10, 10, 10, 10});
        hash.insert(2, AABB{30, 30, 10, 10});
        
        auto collisions = hash.get_potential_collisions();
        
        // Should find the pair (1, 2)
        CHECK(collisions.size() >= 1);
        
        bool found_pair = false;
        for (const auto& pair : collisions) {
            if ((pair.first == 1 && pair.second == 2) || (pair.first == 2 && pair.second == 1)) {
                found_pair = true;
                break;
            }
        }
        CHECK(found_pair);
    }

    TEST_CASE("SpatialHash get_potential_collisions with entities in different cells") {
        SpatialHash hash(64.0f);
        
        // Entity 1 in cell (0, 0)
        hash.insert(1, AABB{10, 10, 10, 10});
        
        // Entity 2 in cell (2, 2)
        hash.insert(2, AABB{150, 150, 10, 10});
        
        auto collisions = hash.get_potential_collisions();
        
        // Should not find any pairs (different cells)
        CHECK(collisions.empty());
    }

    TEST_CASE("SpatialHash handles negative coordinates") {
        SpatialHash hash(64.0f);
        
        hash.insert(1, AABB{-50, -50, 10, 10});
        hash.insert(2, AABB{-30, -30, 10, 10});
        
        CHECK(hash.get_entity_count() == 2);
        
        auto results = hash.query(AABB{-50, -50, 10, 10});
        CHECK(!results.empty());
    }

    TEST_CASE("SpatialHash handles zero-sized entity") {
        SpatialHash hash(64.0f);
        
        hash.insert(1, AABB{10, 10, 0, 0});
        CHECK(hash.get_entity_count() == 1);
        
        auto results = hash.query(AABB{10, 10, 0, 0});
        CHECK(std::find(results.begin(), results.end(), 1) != results.end());
    }

    TEST_CASE("SpatialHash query with large area") {
        SpatialHash hash(64.0f);
        
        // Insert entities in different cells
        hash.insert(1, AABB{10, 10, 10, 10});      // Cell (0, 0)
        hash.insert(2, AABB{70, 10, 10, 10});      // Cell (1, 0)
        hash.insert(3, AABB{10, 70, 10, 10});      // Cell (0, 1)
        hash.insert(4, AABB{70, 70, 10, 10});      // Cell (1, 1)
        hash.insert(5, AABB{200, 200, 10, 10});    // Cell (3, 3)
        
        // Query large area spanning cells (0,0) to (1,1)
        auto results = hash.query(AABB{0, 0, 100, 100});
        
        // Should find entities 1, 2, 3, 4 but not 5
        CHECK(results.size() >= 4);
        CHECK(std::find(results.begin(), results.end(), 5) == results.end());
    }

    TEST_CASE("SpatialHash reinsertion updates position") {
        SpatialHash hash(64.0f);
        
        hash.insert(1, AABB{10, 10, 10, 10});
        
        // Reinsert same entity with different position
        hash.insert(1, AABB{150, 150, 10, 10});
        
        // Should still have only 1 entity
        CHECK(hash.get_entity_count() == 1);
        
        // Should find in new position
        auto results = hash.query(AABB{150, 150, 10, 10});
        CHECK(std::find(results.begin(), results.end(), 1) != results.end());
    }

    TEST_CASE("SpatialHash stress test with many entities") {
        SpatialHash hash(64.0f);
        
        // Insert 100 entities
        for (size_t i = 0; i < 100; i++) {
            float x = (i % 10) * 70.0f; // Spread across cells
            float y = (i / 10) * 70.0f;
            hash.insert(i, AABB{x, y, 10, 10});
        }
        
        CHECK(hash.get_entity_count() == 100);
        
        // Query should return some entities
        auto results = hash.query(AABB{0, 0, 10, 10});
        CHECK(!results.empty());
        
        // Clear should work
        hash.clear();
        CHECK(hash.get_entity_count() == 0);
    }

    TEST_CASE("SpatialHash get_potential_collisions with many entities") {
        SpatialHash hash(64.0f);
        
        // Insert 4 entities in same cell
        hash.insert(1, AABB{10, 10, 5, 5});
        hash.insert(2, AABB{20, 20, 5, 5});
        hash.insert(3, AABB{30, 30, 5, 5});
        hash.insert(4, AABB{40, 40, 5, 5});
        
        auto collisions = hash.get_potential_collisions();
        
        // Should find multiple pairs (1,2), (1,3), (1,4), (2,3), (2,4), (3,4) = 6 pairs
        CHECK(collisions.size() >= 6);
    }
}
