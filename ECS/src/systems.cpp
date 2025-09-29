/*
** EPITECH PROJECT, 2025
** asd
** File description:
** ssad
*/

#include <raylib.h>
#include <iostream>
#include <algorithm>
#include <array>
#include <vector>
#include <cstdlib>


#include "ecs/registry.hpp"
#include "ecs/components.hpp"
#include "ecs/systems.hpp"
#include "ecs/zipper.hpp"
#include "ecs/entity.hpp"

// Position system: for every entity having both position and velocity, add velocity to position.
void position_system(registry &r, float dt) {
    auto *pos_arr = r.get_if<position>();
    auto *vel_arr = r.get_if<velocity>();
    if (!pos_arr || !vel_arr) return;
    for (auto [pos, vel, entity] : zipper(*pos_arr, *vel_arr)) {
        pos.x += vel.vx * dt;
        pos.y += vel.vy * dt;
    }
}

// Control system: for entities with controllable and velocity, set velocity based on keyboard.
void control_system(registry &r) {
    auto *vel_arr = r.get_if<velocity>();
    auto *ctrl_arr = r.get_if<controllable>();
    if (!vel_arr || !ctrl_arr) return;
    for (auto [vel, ctrl, entity] : zipper(*vel_arr, *ctrl_arr)) {
        float speed = ctrl.speed;
        float vx = 0.f, vy = 0.f;
        if (IsKeyDown(KEY_RIGHT)) vx += speed;
        if (IsKeyDown(KEY_LEFT))  vx -= speed;
        if (IsKeyDown(KEY_DOWN))  vy += speed;
        if (IsKeyDown(KEY_UP))    vy -= speed;
        vel.vx = vx;
        vel.vy = vy;
    }
}

// Draw system: for entities with position and drawable, render rectangles.
void draw_system(registry &r) {
    auto *pos_arr = r.get_if<position>();
    auto *draw_arr = r.get_if<drawable>();
    if (!pos_arr || !draw_arr) return;
    for (auto [p, d, entity] : zipper(*pos_arr, *draw_arr)) {
        DrawRectangle((int)p.x, (int)p.y, (int)d.w, (int)d.h,
                     (Color){d.r, d.g, d.b, d.a});
    }
}

// file-local helpers (Utility: Collision Systeme)
using Rect = std::array<float,4>; // {minx, miny, maxx, maxy}

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

// Simple AABB collision detection & resolution system.
void collision_system(registry &r) {
    auto *pos_arr = r.get_if<position>();
    auto *col_arr = r.get_if<collider>();
    if (!pos_arr || !col_arr) return;
    for (auto [pi, ci, entity_i] : zipper(*pos_arr, *col_arr)) {
        Rect a = make_rect(pi, ci);
        for (auto [pj, cj, entity_j] : zipper(*pos_arr, *col_arr)) {
            if (entity_i == entity_j) continue;
            Rect b = make_rect(pj, cj);
            if (!rect_overlap(a, b)) continue;
            if (ci.is_trigger || cj.is_trigger) {
                std::cerr << "Trigger collision: " << entity_i << " <-> " << entity_j << "\n";
                continue;
            }
            resolve_penetration(pi, a, pj, b);
        }
    }
}

// Spawn system: DISABLED - enemies are now spawned server-side
// Client-side spawning would cause desynchronization in multiplayer
void spawn_system(registry &r, float dt, int window_width, int window_height) {
    // This function is disabled for multiplayer synchronization
    // Enemies are now managed by the server and sent to clients via ENTITY_UPDATE messages
    return;
}

// Lifetime system: manages entity lifetime and removes expired entities
void lifetime_system(registry &r, float dt, int window_width, int window_height) {
    auto *lifetime_arr = r.get_if<lifetime>();
    auto *pos_arr = r.get_if<position>();
    auto *spawner_arr = r.get_if<spawner>();

    if (!lifetime_arr || !pos_arr) return;

    std::vector<entity> entities_to_remove;

    for (auto [life_comp, pos_comp, ent] : zipper(*lifetime_arr, *pos_arr)) {
        life_comp.current_time += dt;
        
        bool should_destroy = false;
        
        // Check if lifetime expired
        if (life_comp.current_time >= life_comp.max_lifetime) {
            should_destroy = true;
        }
        
        // Check if entity is offscreen and should be destroyed
        if (life_comp.destroy_offscreen) {
            if (pos_comp.x < -100 || pos_comp.x > window_width + 100 ||
                pos_comp.y < -100 || pos_comp.y > window_height + 100) {
                should_destroy = true;
            }
        }
        
        if (should_destroy) {
            entities_to_remove.push_back(entity(ent));
        }
    }    // Remove expired entities and update spawner counts
    for (entity ent : entities_to_remove) {
        // If entity was an enemy, decrease spawner count
        if (r.get_if<enemy>() && r.get_if<enemy>()->size() > static_cast<size_t>(ent)) {
            if (spawner_arr) {
                for (auto [spawn_comp, spawn_entity] : zipper(*spawner_arr)) {
                    spawn_comp.current_count = std::max(0, spawn_comp.current_count - 1);
                }
            }
        }

        r.kill_entity(ent);
    }
}
