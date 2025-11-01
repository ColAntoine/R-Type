/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** CurrentWave component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct CurrentWave : public IComponent {
    int _currentWave{0};

    CurrentWave();
    CurrentWave(int currentWave);
};
