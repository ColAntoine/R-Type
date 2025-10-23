/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Invincibility component implementation
*/

#include "Invincibility.hpp"

Invincibility::Invincibility() {}
Invincibility::Invincibility(float duration, bool isInvisible, float lastActivation)
: _duration(duration), _isInvincible(isInvisible), _lastActivation(lastActivation)
{}
