/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** SpatialHash - Spatial hashing for O(n) collision detection
*/

#pragma once

#include "ECS/Physics/Collision/CollisionDetector.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>

class SpatialHash {
    public:
        SpatialHash(float cell_size = 64.0f);

        void clear();
        void insert(size_t entity_id, const AABB& bounds);
        void remove(size_t entity_id);

        std::vector<size_t> query(const AABB& bounds) const;
        std::vector<std::pair<size_t, size_t>> get_potential_collisions() const;
        void set_cell_size(float size) { cell_size_ = size; }
        float get_cell_size() const { return cell_size_; }
        size_t get_entity_count() const { return entity_bounds_.size(); }

    private:
        struct CellCoord {
            int x, y;

            bool operator==(const CellCoord& other) const {
                return x == other.x && y == other.y;
            }
        };

        struct CellCoordHash {
            size_t operator()(const CellCoord& coord) const {
                return std::hash<int>()(coord.x) ^ (std::hash<int>()(coord.y) << 1);
            }
        };

        CellCoord get_cell(float x, float y) const;
        std::vector<CellCoord> get_cells(const AABB& bounds) const;

    float cell_size_;
    std::unordered_map<CellCoord, std::unordered_set<size_t>, CellCoordHash> grid_;
    std::unordered_map<size_t, AABB> entity_bounds_;
};