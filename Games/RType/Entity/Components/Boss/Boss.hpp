/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Boss component
*/

#pragma once

#include "ECS/Components/IComponent.hpp"

#include <string>

struct Boss : public IComponent {
    std::string _armSpritePath{""};
    std::string _bodySpritePath{""};
    std::string _bulletSpritePath{""};
    int _wave{1};

    Boss();
    Boss(std::string armPath, std::string bodyPath, std::string bulletPath, int wave);
};
