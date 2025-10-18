/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Player component implementation
*/

#include "Player.hpp"

Player::Player() {}
Player::Player(int life, float cooldow, float cCooldown, int score, float invincibilty)
: _life(life),
_cooldown(_cooldown),
_currentCooldown(cCooldown),
_score(score),
_invincibility(invincibilty)
{}
