/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** Camera Implementation
*/

#include "ECS/Renderer/Camera/Camera.hpp"

Camera2D_ECS::Camera2D_ECS()
    : offset_{0, 0}
    , target_{0, 0}
    , rotation_(0.0f)
    , zoom_(1.0f) {
}

Camera2D_ECS::Camera2D_ECS(float x, float y, float zoom)
    : offset_{0, 0}
    , target_{x, y}
    , rotation_(0.0f)
    , zoom_(zoom) {
}

void Camera2D_ECS::set_position(float x, float y) {
    target_.x = x;
    target_.y = y;
}

void Camera2D_ECS::set_zoom(float zoom) {
    if (zoom > 0.1f && zoom < 10.0f) {
        zoom_ = zoom;
    }
}

void Camera2D_ECS::move(float dx, float dy) {
    target_.x += dx;
    target_.y += dy;
}

void Camera2D_ECS::zoom_in(float amount) {
    zoom_ += amount;
    if (zoom_ > 10.0f) zoom_ = 10.0f;
}

void Camera2D_ECS::zoom_out(float amount) {
    zoom_ -= amount;
    if (zoom_ < 0.1f) zoom_ = 0.1f;
}

Camera2D Camera2D_ECS::get_raylib_camera() const {
    Camera2D cam;
    cam.offset = offset_;
    cam.target = target_;
    cam.rotation = rotation_;
    cam.zoom = zoom_;
    return cam;
}

void Camera2D_ECS::begin_mode() {
    BeginMode2D(get_raylib_camera());
}

void Camera2D_ECS::end_mode() {
    EndMode2D();
}

Vector2 Camera2D_ECS::screen_to_world(Vector2 screen_pos) const {
    return GetScreenToWorld2D(screen_pos, get_raylib_camera());
}

Vector2 Camera2D_ECS::world_to_screen(Vector2 world_pos) const {
    return GetWorldToScreen2D(world_pos, get_raylib_camera());
}
