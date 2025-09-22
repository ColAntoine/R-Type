/*
** EPITECH PROJECT, 2025
** asd
** File description:
** asd
*/

#ifndef asd
#define asd

#include <SFML/Graphics.hpp>

#include "ecs/registry.hpp"
#include "ecs/components.hpp"

// Position system: for every entity having both position and velocity, add velocity to position.
void position_system(registry &r, float dt);

// Control system: for entities with controllable and velocity, set velocity based on keyboard.
void control_system(registry &r);

// Draw system: for entities with position and drawable, render rectangles onto the target.
void draw_system(registry &r, sf::RenderTarget &target);

// Simple AABB collision detection & resolution system.
// Iterates all pairs of entities that have position + collider. For non-trigger colliders, resolves penetration
// by moving the first entity out of overlap along the smallest axis.
void collision_system(registry &r);

#endif /* !asd */
