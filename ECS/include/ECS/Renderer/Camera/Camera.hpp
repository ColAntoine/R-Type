/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** Camera2D - 2D camera system for viewport control
*/

#pragma once

#include <raylib.h>

class Camera2D_ECS {
public:
    Camera2D_ECS();
    Camera2D_ECS(float x, float y, float zoom = 1.0f);

    void set_position(float x, float y);
    void set_zoom(float zoom);
    void move(float dx, float dy);
    void zoom_in(float amount);
    void zoom_out(float amount);

    Vector2 get_position() const { return {offset_.x, offset_.y}; }
    float get_zoom() const { return zoom_; }

    Camera2D get_raylib_camera() const;

    void begin_mode();
    void end_mode();

    Vector2 screen_to_world(Vector2 screen_pos) const;
    Vector2 world_to_screen(Vector2 world_pos) const;

private:
    Vector2 offset_;
    Vector2 target_;
    float rotation_;
    float zoom_;
};
