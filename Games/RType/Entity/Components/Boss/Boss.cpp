/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Boss component implementation
*/

#include "Boss.hpp"

Boss::Boss() {}
Boss::Boss(std::string armPath, std::string bodyPath, std::string bulletPath)
: _armSpritePath(armPath), _bodySpritePath(bodyPath), _bulletSpritePath(bulletPath)
{}
