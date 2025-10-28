/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** DeathAnimation component implementation
*/

#include "DeathAnimation.hpp"

DeathAnimation::DeathAnimation() {}
DeathAnimation::DeathAnimation(std::string spritePath, float duration, float elapsed)
: _spritePath{spritePath}, _duration(duration), _elapsed(elapsed)
{}