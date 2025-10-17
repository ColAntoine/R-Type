/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** PhysicsManager - Central physics system (singleton)
*/

#pragma once

#include "Collision/CollisionDetector.hpp"
#include "SpatialHash/SpatialHash.hpp"

class PhysicsManager {
    public:
        static PhysicsManager& instance();

        void init(float cell_size = 64.0f);
        void clear();

    SpatialHash& get_spatial_hash() { return spatial_hash_; }
    const SpatialHash& get_spatial_hash() const { return spatial_hash_; }

        void update_entity(size_t entity_id, const AABB& bounds);
        void remove_entity(size_t entity_id);

        std::vector<size_t> query_area(const AABB& bounds) const;
        std::vector<std::pair<size_t, size_t>> get_collision_pairs() const;
        bool check_collision(const AABB& a, const AABB& b) const;
        bool check_collision_circle(Vector2 center1, float radius1,
                                    Vector2 center2, float radius2) const;

    private:
        PhysicsManager() : spatial_hash_(64.0f) {}
        ~PhysicsManager() = default;
        PhysicsManager(const PhysicsManager&) = delete;
        PhysicsManager& operator=(const PhysicsManager&) = delete;

        SpatialHash spatial_hash_;
};
