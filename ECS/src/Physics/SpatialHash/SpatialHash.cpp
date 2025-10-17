/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** SpatialHash Implementation
*/

#include "ECS/Physics/SpatialHash/SpatialHash.hpp"
#include <cmath>
#include <algorithm>

SpatialHash::SpatialHash(float cell_size)
    : cell_size_(cell_size) {
}

void SpatialHash::clear() {
    grid_.clear();
    entity_bounds_.clear();
}

SpatialHash::CellCoord SpatialHash::get_cell(float x, float y) const {
    return {
        static_cast<int>(std::floor(x / cell_size_)),
        static_cast<int>(std::floor(y / cell_size_))
    };
}

std::vector<SpatialHash::CellCoord> SpatialHash::get_cells(const AABB& bounds) const {
    std::vector<CellCoord> cells;

    CellCoord min_cell = get_cell(bounds.x, bounds.y);
    CellCoord max_cell = get_cell(bounds.x + bounds.width, bounds.y + bounds.height);

    for (int x = min_cell.x; x <= max_cell.x; ++x) {
        for (int y = min_cell.y; y <= max_cell.y; ++y) {
            cells.push_back({x, y});
        }
    }

    return cells;
}

void SpatialHash::insert(size_t entity_id, const AABB& bounds) {
    remove(entity_id);

    entity_bounds_[entity_id] = bounds;

    auto cells = get_cells(bounds);
    for (const auto& cell : cells) {
        grid_[cell].insert(entity_id);
    }
}

void SpatialHash::remove(size_t entity_id) {
    auto it = entity_bounds_.find(entity_id);
    if (it == entity_bounds_.end()) return;

    auto cells = get_cells(it->second);
    for (const auto& cell : cells) {
        auto grid_it = grid_.find(cell);
        if (grid_it != grid_.end()) {
            grid_it->second.erase(entity_id);
            if (grid_it->second.empty()) {
                grid_.erase(grid_it);
            }
        }
    }

    entity_bounds_.erase(it);
}std::vector<size_t> SpatialHash::query(const AABB& bounds) const {
    std::unordered_set<size_t> result_set;

    auto cells = get_cells(bounds);
    for (const auto& cell : cells) {
        auto it = grid_.find(cell);
        if (it != grid_.end()) {
            for (size_t entity_id : it->second) {
                result_set.insert(entity_id);
            }
        }
    }

    return std::vector<size_t>(result_set.begin(), result_set.end());
}

std::vector<std::pair<size_t, size_t>> SpatialHash::get_potential_collisions() const {
    std::vector<std::pair<size_t, size_t>> pairs;
    std::unordered_set<size_t> checked;

    for (const auto& [cell, entities] : grid_) {
        std::vector<size_t> entity_list(entities.begin(), entities.end());

        for (size_t i = 0; i < entity_list.size(); ++i) {
            for (size_t j = i + 1; j < entity_list.size(); ++j) {
                size_t id1 = entity_list[i];
                size_t id2 = entity_list[j];

                if (id1 > id2) std::swap(id1, id2);

                size_t pair_hash = id1 * 100000 + id2;
                if (checked.find(pair_hash) == checked.end()) {
                    checked.insert(pair_hash);
                    pairs.push_back({id1, id2});
                }
            }
        }
    }

    return pairs;
}