/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Score component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

struct Score : public IComponent {
    unsigned _score{0};

    Score();
    Score(unsigned score=0);
};
