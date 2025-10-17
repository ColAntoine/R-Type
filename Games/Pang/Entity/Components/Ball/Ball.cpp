/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Ball component implementation
*/

#include "Ball.hpp"

Ball::Ball() {}
Ball::Ball(float radius, Color color, bool isACtive) 
: _radius(radius), _color(color), _isActive(isACtive)
{}
