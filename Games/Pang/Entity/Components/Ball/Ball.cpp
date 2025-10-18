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
    factory->create_component<collider>(r, ballEntity);
    factory->create_component<Gravity>(r, ballEntity, GRAVITY);
}
