/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PUpAnimation component implementation
*/

#include "PUpAnimation.hpp"

PUpAnimation::PUpAnimation() {}
PUpAnimation::PUpAnimation(bool up, std::string text)
: _up(up), _text(text)
{}
PUpAnimation::PUpAnimation(bool up, std::string text, float duration)
: _up(up), _text(text), _duration(duration)
{}
