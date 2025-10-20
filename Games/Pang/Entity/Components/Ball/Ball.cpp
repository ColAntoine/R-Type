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
    auto ballEntity = r.spawn_entity();
    factory->create_component<position>(r, ballEntity, pos.x, pos.y);
    factory->create_component<velocity>(r, ballEntity, 0.0f, 0.0f);
    factory->create_component<Ball>(r, ballEntity, 20.0f, RED, true, type);
    float diameter = 20.0f * 2.0f;
    factory->create_component<collider>(r, ballEntity,
        diameter, diameter,
        -20.0f, -20.0f);
    factory->create_component<Gravity>(r, ballEntity, GRAVITY);
}
