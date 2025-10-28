/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PowerUp component implementation
*/

#include "PowerUp.hpp"

PowerUp::PowerUp() {}
PowerUp::PowerUp(powerUpType pwType, float w, float h, float ox, float oy)
: _pwType(pwType), _w(w), _h(h), _offset_x(ox), _offset_y(oy)
{}
