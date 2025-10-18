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
Ball::Ball(float radius, Color color, bool isACtive)
: _radius(radius), _color(color), _isActive(isACtive)
{}

void Ball::spawn(IComponentFactory *factory, registry &r, position pos)
{
    auto ballEntity = r.spawn_entity();
    factory->create_component<position>(r, ballEntity, pos.x, pos.y);
    factory->create_component<velocity>(r, ballEntity, 0.0f, 0.0f);
    factory->create_component<Ball>(r, ballEntity, 20.0f, RED, true);
    
    // Create collider with proper size based on ball radius
    // Width and height should be diameter (2 * radius)
    // Offset to center the collider on the ball position
    float diameter = 20.0f * 2.0f; // 2 * radius
    factory->create_component<collider>(r, ballEntity, 
        diameter, diameter,           // width, height (diameter)
        -20.0f, -20.0f);             // offset to center (-radius, -radius)
    
    factory->create_component<Gravity>(r, ballEntity, GRAVITY);
}
