/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** animation component for horizontal spritesheets
*/

#pragma once

#include "ECS/Components/IComponent.hpp"
#include <string>

struct animation : public IComponent {
    // Spritesheet resource path (relative path to assets/)
    std::string texture_path;

    // Frame dimensions in pixels
    float frame_width{400}; // Default frame width
    float frame_height{400}; // Default frame height

    // Scaling factors
    float scale_x{1.0f}; // Horizontal scale (1.0 = normal size)
    float scale_y{1.0f}; // Vertical scale (1.0 = normal size)

    // Number of frames horizontally (optional: if 0 we will compute at runtime)
    int frame_count{0};
    // Current playback frame (0-based)
    int current_frame{0};
    // Seconds per frame
    float frame_time{0.1f};
    // Internal timer for frame timing
    float frame_timer{0.0f};
    // Whether animation loops back to start after last frame
    bool loop{true};
    // If true the animation only advances while the entity is moving (velocity != 0)
    bool play_on_movement{false};


    animation() = default;
    animation(const std::string &path, float fw, float fh, int fc = 0, bool playOnMovement = false)
        : texture_path(path), frame_width(fw), frame_height(fh), frame_count(fc), current_frame(0), frame_time(0.1f), frame_timer(0.0f), loop(true), play_on_movement(playOnMovement) {}
    animation(const std::string &path, float fw, float fh, float sx, float sy, int fc = 0, bool playOnMovement = false)
        : texture_path(path), frame_width(fw), frame_height(fh), scale_x(sx), scale_y(sy), frame_count(fc), current_frame(0), frame_time(0.1f), frame_timer(0.0f), loop(true), play_on_movement(playOnMovement) {}
};
