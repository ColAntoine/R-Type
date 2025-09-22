/*
** EPITECH PROJECT, 2025
** asd
** File description:
** ssad
*/

#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <array>


#include "ecs/registry.hpp"
#include "ecs/components.hpp"
#include "ecs/systems.hpp"
#include "ecs/zipper.hpp"

// Position system: for every entity having both position and velocity, add velocity to position.
void position_system(registry &r) {
    auto *pos_arr = r.get_if<position>();
    auto *vel_arr = r.get_if<velocity>();
    if (!pos_arr || !vel_arr) return;
    for (auto [pos, vel, entity] : zipper(*pos_arr, *vel_arr)) {
        pos.x += vel.vx;
        pos.y += vel.vy;
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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) vx += speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  vx -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  vy += speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    vy -= speed;
        vel.vx = vx;
        vel.vy = vy;
    }
}

// Draw system: for entities with position and drawable, render rectangles onto the target.
void draw_system(registry &r, sf::RenderTarget &target) {
    auto *pos_arr = r.get_if<position>();
    auto *draw_arr = r.get_if<drawable>();
    if (!pos_arr || !draw_arr) return;
    sf::RectangleShape shape;
    for (auto [p, d, entity] : zipper(*pos_arr, *draw_arr)) {
        shape.setSize(sf::Vector2f(d.w, d.h));
        shape.setPosition(p.x, p.y);
        shape.setFillColor(sf::Color(d.r, d.g, d.b, d.a));
        target.draw(shape);
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
