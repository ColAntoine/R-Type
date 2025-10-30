/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Wave component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

#include "ECS/Components/Position.hpp"

struct WaveShoot : public IComponent {
    int _projNb{5};
    float _expTimeout{5.f};
    float _lifeTime{0.f};
    bool _didExplode{false};
    position _stopedPos{0.f, 0.f};
    bool _isStoped{false};

    WaveShoot();
    WaveShoot(int projNumber, float expTimeout);
};
