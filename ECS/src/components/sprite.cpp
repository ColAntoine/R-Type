/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** sprite
*/

#include "ECS/Components/Sprite.hpp"

sprite::sprite() : texture_path(""), width(64.0f), height(64.0f) {}

sprite::sprite(const std::string& path, float w, float h)
    : texture_path(path), width(w), height(h) {}

sprite::sprite(const std::string& path, float w, float h, float rota)
    : texture_path(path), width(w), height(h), rotation(rota) {}

sprite::sprite(const std::string& path, float w, float h, float sx, float sy)
    : texture_path(path), width(w), height(h), scale_x(sx), scale_y(sy) {}
