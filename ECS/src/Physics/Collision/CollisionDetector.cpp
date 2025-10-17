/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** CollisionDetector Implementation
*/

#include "ECS/Physics/Collision/CollisionDetector.hpp"
#include <cmath>

bool AABB::intersects(const AABB& other) const {
    return !(x + width < other.x ||
             other.x + other.width < x ||
             y + height < other.y ||
             other.y + other.height < y);
}

bool AABB::contains(float px, float py) const {
    return px >= x && px <= x + width &&
           py >= y && py <= y + height;
}

Vector2 AABB::get_center() const {
    return {x + width / 2.0f, y + height / 2.0f};
}

bool CollisionDetector::check_aabb(const AABB& a, const AABB& b) {
    return a.intersects(b);
}

bool CollisionDetector::check_circle(Vector2 center1, float radius1,
                                     Vector2 center2, float radius2) {
    float dx = center2.x - center1.x;
    float dy = center2.y - center1.y;
    float distance_squared = dx * dx + dy * dy;
    float radius_sum = radius1 + radius2;
    return distance_squared <= radius_sum * radius_sum;
}

bool CollisionDetector::check_point_in_rect(Vector2 point, Rectangle rect) {
    return point.x >= rect.x && point.x <= rect.x + rect.width &&
           point.y >= rect.y && point.y <= rect.y + rect.height;
}

bool CollisionDetector::check_rect_overlap(Rectangle rect1, Rectangle rect2) {
    return CheckCollisionRecs(rect1, rect2);
}
