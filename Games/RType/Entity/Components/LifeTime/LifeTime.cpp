/*
** EPITECH PROJECT, 2025
** asd
** File description:
** da
*/

#include "Entity/Components/LifeTime/LifeTime.hpp"

lifetime::lifetime() {}
lifetime::lifetime(float max_time, bool collision_destroy, bool offscreen_destroy)
    : max_lifetime(max_time), destroy_on_collision(collision_destroy), destroy_offscreen(offscreen_destroy) {}
