/*
** EPITECH PROJECT, 2025
** asd
** File description:
** da
*/

#include "ecs/components/collider.hpp"

collider::collider() {}
collider::collider(float nw, float nh, float ox, float oy, bool trig) : w(nw), h(nh), offset_x(ox), offset_y(oy), is_trigger(trig) {}
