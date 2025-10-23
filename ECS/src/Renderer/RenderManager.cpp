/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** RenderManager Implementation
*/

#include "ECS/Renderer/RenderManager.hpp"
#include <iostream>

RenderManager& RenderManager::instance() {
    static RenderManager instance;
    return instance;
}

void RenderManager::init(int width, int height, const char* title) {
    // Only create window if it doesn't exist yet
    if (!IsWindowReady()) {
        InitWindow(width, height, title);
        SetTargetFPS(60);
        std::cout << "RenderManager: Created new window " << width << "x" << height << std::endl;
    } else {
        std::cout << "RenderManager: Using existing window" << std::endl;
    }

    // For Camera2D: offset = screen viewport center, target = world point to look at
    // To make world coordinates match screen coordinates (0,0 = top-left):
    // We don't use camera transform - just set offset=target=0 or disable camera
    camera_.set_position(0.0f, 0.0f);

    std::cout << "RenderManager initialized (camera disabled for 1:1 screen mapping)" << std::endl;
}

void RenderManager::shutdown() {
    CloseWindow();
    std::cout << "RenderManager shutdown" << std::endl;
}

void RenderManager::begin_frame() {
    BeginDrawing();
    ClearBackground(clear_color_);
    // Disable camera for now - use direct screen coordinates
    // camera_.begin_mode();
    batch_.begin();
}

void RenderManager::end_frame() {
    batch_.end();
    batch_.flush();
    // camera_.end_mode();
    EndDrawing();
}

bool RenderManager::should_close() const {
    return WindowShouldClose();
}

int RenderManager::get_fps() const {
    return GetFPS();
}

void RenderManager::draw_text(const char *text, int posX, int posY, int fontSize, Color color) const {
    return DrawText(text, posX, posY, fontSize, color);
}

bool RenderManager::is_window_ready() const {
    return IsWindowReady();
}

bool RenderManager::window_should_close() const {
    return WindowShouldClose();
}
