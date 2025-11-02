/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Collision System Implementation - Using Spatial Hash for O(n) performance
*/

#include <iostream>
#include <algorithm>
#include <array>
#include <raylib.h>
#include "ECS/Systems/Collision.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Physics/PhysicsManager.hpp"

using Rect = std::array<float,4>; // {minx, miny, maxx, maxy}

// TODO: create a new system for the penetration maybe call it impactable or smth
// TODO: but the collider system should only turn the boolean to true
// (move the respolve pene function to a new sys and add a component for it)
static inline Rect make_rect(const position &p, const collider &c)
{
    float x = p.x + c.offset_x;
    float y = p.y + c.offset_y;
    return { x, y, x + c.w, y + c.h };
}

static inline bool rect_overlap(const Rect &a, const Rect &b)
{
    return !(a[2] < b[0] || a[0] > b[2] || a[3] < b[1] || a[1] > b[3]);
}

static inline void resolve_penetration(position &pi, const Rect &a, position & /*pj*/, const Rect &b)
{
    float overlap_x = std::min(a[2], b[2]) - std::max(a[0], b[0]);
    float overlap_y = std::min(a[3], b[3]) - std::max(a[1], b[1]);

    if (overlap_x < overlap_y) {
        if (a[0] < b[0]) pi.x -= overlap_x; else pi.x += overlap_x;
    } else {
        if (a[1] < b[1]) pi.y -= overlap_y; else pi.y += overlap_y;
    }
}

void CollisionSystem::update(registry& r, float dt) {
    auto *pos_arr = r.get_if<position>();
    auto *col_arr = r.get_if<collider>();
    if (!pos_arr || !col_arr) return;

    auto& physics = PhysicsManager::instance();
    physics.clear();

    // Step 1: Insert all entities with colliders into spatial hash
    for (auto [pi, ci, entity_i] : zipper(*pos_arr, *col_arr)) {
        AABB bounds(pi.x + ci.offset_x, pi.y + ci.offset_y, ci.w, ci.h);
        physics.update_entity(entity_i, bounds);
    }

    // Step 2: Get only potential collision pairs (O(n) instead of O(n²))
    auto collision_pairs = physics.get_collision_pairs();

    // Step 3: Check and resolve actual collisions
    for (auto [entity_i, entity_j] : collision_pairs) {
        if (!pos_arr->has(static_cast<size_t>(entity_i)) || !col_arr->has(static_cast<size_t>(entity_i)) ||
            !pos_arr->has(static_cast<size_t>(entity_j)) || !col_arr->has(static_cast<size_t>(entity_j))) {
            continue;
        }

        auto& pi = pos_arr->get(static_cast<size_t>(entity_i));
        auto& ci = col_arr->get(static_cast<size_t>(entity_i));
        auto& pj = pos_arr->get(static_cast<size_t>(entity_j));
        auto& cj = col_arr->get(static_cast<size_t>(entity_j));

        Rect a = make_rect(pi, ci);
        Rect b = make_rect(pj, cj);

        if (!rect_overlap(a, b)) continue;

        if (ci.is_trigger || cj.is_trigger) {
            std::cerr << "Trigger collision: " << entity_i << " <-> " << entity_j << "\n";
            continue;
        }

        resolve_penetration(pi, a, pj, b);
    }
}

DLL_EXPORT ISystem* create_system() {
    try {
        return new CollisionSystem();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}