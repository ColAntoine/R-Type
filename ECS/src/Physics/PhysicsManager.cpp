/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** PhysicsManager Implementation
*/

#include "ECS/Physics/PhysicsManager.hpp"
#include <iostream>

PhysicsManager& PhysicsManager::instance() {
    static PhysicsManager instance;
    return instance;
}

void PhysicsManager::init(float cell_size) {
    spatial_hash_.set_cell_size(cell_size);
    spatial_hash_.clear();
    std::cout << "PhysicsManager initialized (cell_size=" << cell_size << ")" << std::endl;
}

void PhysicsManager::clear() {
    spatial_hash_.clear();
}

void PhysicsManager::update_entity(size_t entity_id, const AABB& bounds) {
    spatial_hash_.insert(entity_id, bounds);
}

void PhysicsManager::remove_entity(size_t entity_id) {
    spatial_hash_.remove(entity_id);
}

std::vector<size_t> PhysicsManager::query_area(const AABB& bounds) const {
    return spatial_hash_.query(bounds);
}

std::vector<std::pair<size_t, size_t>> PhysicsManager::get_collision_pairs() const {
    return spatial_hash_.get_potential_collisions();
}

bool PhysicsManager::check_collision(const AABB& a, const AABB& b) const {
    return CollisionDetector::check_aabb(a, b);
}

bool PhysicsManager::check_collision_circle(Vector2 center1, float radius1, 
                                           Vector2 center2, float radius2) const {
    return CollisionDetector::check_circle(center1, radius1, center2, radius2);
}
