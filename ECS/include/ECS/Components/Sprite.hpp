/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** sprite
*/

#pragma once

#include "ECS/Components/IComponent.hpp"
#include <string>

struct sprite : public IComponent {
    std::string texture_path;
    float width{64.0f}, height{64.0f};
    float scale_x{1.0f}, scale_y{1.0f};
    float rotation{0.0f};
    int frame_x{0}, frame_y{0};
    bool visible{true};

    sprite();
    sprite(const std::string& path, float w, float h);
    sprite(const std::string& path, float w, float h, float sx, float sy);
};
