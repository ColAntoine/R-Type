/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** CollisionDetector - Collision detection utilities
*/

#pragma once

#include <raylib.h>

struct AABB {
    float x, y;
    float width, height;

    AABB() : x(0), y(0), width(0), height(0) {}
    AABB(float x, float y, float w, float h) : x(x), y(y), width(w), height(h) {}

    bool intersects(const AABB& other) const;
    bool contains(float px, float py) const;
    Vector2 get_center() const;
};

class CollisionDetector {
    public:
        static bool check_aabb(const AABB& a, const AABB& b);
        static bool check_circle(Vector2 center1, float radius1,
                                Vector2 center2, float radius2);
        static bool check_point_in_rect(Vector2 point, Rectangle rect);
        static bool check_rect_overlap(Rectangle rect1, Rectangle rect2);
};
