/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Ball component implementation
*/

#include "Ball.hpp"
#include "ECS/Components.hpp"
#include "Entity/Components/Gravity/Gravity.hpp"

Ball::Ball() {}
Ball::Ball(float radius, Color color, bool isACtive, ballType type)
: _radius(radius), _color(color), _isActive(isACtive), _type(type)
{
    if (type == CUSTOM) return;

    if (type == SMALL) {
        _radius = 25.f;
    } else if (type == MEDIUM) {
        _radius = 50.f;
    } else if (type == LARGE) {
        _radius = 100.f;
    }
}

void Ball::spawn(IComponentFactory *factory, registry &r, position pos, ballType type)
{
    // Determine radius based on ball type
    float radius;
    if (type == SMALL) {
        radius = 25.f;
    } else if (type == MEDIUM) {
        radius = 50.f;
    } else if (type == LARGE) {
        radius = 100.f;
    } else {
        radius = 20.f; // Default for CUSTOM
    }
    
    auto ballEntity = r.spawn_entity();
    r.emplace_component<position>(ballEntity, pos.x, pos.y);
    r.emplace_component<velocity>(ballEntity, 0.0f, 0.0f);
    r.emplace_component<Ball>(ballEntity, radius, RED, true, type);
    
    float diameter = radius * 2.0f;
    r.emplace_component<collider>(ballEntity,
        diameter, diameter,
        -radius, -radius);
    r.emplace_component<Gravity>(ballEntity, GRAVITY);
}

void Ball::spawn(IComponentFactory *factory, registry &r, position pos, ballType type, float vx, float vy)
{
    // Determine radius based on ball type
    float radius;
    if (type == SMALL) {
        radius = 25.f;
    } else if (type == MEDIUM) {
        radius = 50.f;
    } else if (type == LARGE) {
        radius = 100.f;
    } else {
        radius = 20.f; // Default for CUSTOM
    }
    
    auto ballEntity = r.spawn_entity();
    r.emplace_component<position>(ballEntity, pos.x, pos.y);
    r.emplace_component<velocity>(ballEntity, vx, vy);
    r.emplace_component<Ball>(ballEntity, radius, RED, true, type);
    
    float diameter = radius * 2.0f;
    r.emplace_component<collider>(ballEntity,
        diameter, diameter,
        -radius, -radius);
    r.emplace_component<Gravity>(ballEntity, GRAVITY);
}
