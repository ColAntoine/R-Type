/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Parabol component implementation
*/

#include "Parabol.hpp"

Parabol::Parabol() {}
Parabol::Parabol(float x, float y, float range, bool down)
: _startX(x),
_startY(y),
_range(range),
_flickedUp(down)
{}
