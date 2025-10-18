/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Player component implementation
*/

#include "Player.hpp"

Player::Player() {}
Player::Player(int life, float cooldow, float cCooldown,
    int score, float moveSpeed)
: _life(life),
_cooldown(_cooldown),
_currentCooldown(cCooldown),
_score(score),
_moveSpeed(moveSpeed)
{}

void Player::spawn(IComponentFactory *factory, registry &r, position pos)
{
    auto player = r.spawn_entity();

    factory->create_component<position>(r, player, pos.x, pos.y);
    factory->create_component<velocity>(r, player);
    factory->create_component<Player>(r, player);
    factory->create_component<collider>(r, player, 100.f, 100.f, -50.f, -50.f);
    factory->create_component<Invincibility>(r, player, 2.0f, false, 0.0f); // 2 second invincibility
}
