/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Rope component implementation
*/

#include "Rope.hpp"

Rope::Rope() {}

Rope::Rope(float width, float speed, float startY, Color color)
    : _width(width), _speed(speed), _startY(startY), _currentTipY(startY), _color(color)
{}
