/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** RenderManager Implementation
*/

#include "ECS/Renderer/RenderManager.hpp"
#include <iostream>

RenderManager& RenderManager::instance()
{
    static RenderManager instance;
    return instance;
}

int RenderManager::scalePosX(int percent) const
{
    return static_cast<int>((percent / 100.0f) * this->_winInfos.getWidth());
}

int RenderManager::scalePosY(int percent) const
{
    return static_cast<int>((percent / 100.0f) * this->_winInfos.getHeight());
}

int RenderManager::scaleSizeW(int percent) const
{
    return static_cast<int>((percent / 100.0f) * this->_winInfos.getWidth());
}

int RenderManager::scaleSizeH(int percent) const
{
    return static_cast<int>((percent / 100.0f) * this->_winInfos.getHeight());
}

void RenderManager::init(const char *title)
{
    int monitor = 0;
    float scale = 0.8f; // Useful for testing on smaller screens, in production set to 1.0f

    if (!IsWindowReady()) {
        InitWindow(100, 100, "Temp init");

        monitor = GetCurrentMonitor();
        this->_winInfos.setHeight(GetMonitorHeight(monitor) * scale);
        this->_winInfos.setWidth(GetMonitorWidth(monitor) * scale);
        this->_winInfos.setFps(GetMonitorRefreshRate(monitor));

        CloseWindow(); // Close temp init

        InitWindow(this->_winInfos.getWidth(), this->_winInfos.getHeight(), title);
        SetTargetFPS(this->_winInfos.getFps());

        std::cout << "RenderManager: Created new window " << this->_winInfos.getWidth() << "x" << this->_winInfos.getHeight() << std::endl;
    } else {
        std::cout << "RenderManager: Using existing window" << std::endl;
    }
    // For Camera2D: offset = screen viewport center, target = world point to look at
    // To make world coordinates match screen coordinates (0,0 = top-left):
    // We don't use camera transform - just set offset=target=0 or disable camera
    camera_.set_position(0.0f, 0.0f);
    std::cout << "RenderManager initialized (camera disabled for 1:1 screen mapping)" << std::endl;
}

void RenderManager::shutdown()
{
    CloseWindow();
    std::cout << "RenderManager shutdown" << std::endl;
}

void RenderManager::begin_frame()
{
    BeginDrawing();
    ClearBackground(clear_color_);
    // Disable camera for now - use direct screen coordinates
    // camera_.begin_mode();
    batch_.begin();
}

void RenderManager::end_frame()
{
    batch_.end();
    batch_.flush();
    // camera_.end_mode();
    EndDrawing();
}

bool RenderManager::should_close() const
{
    return WindowShouldClose();
}

int RenderManager::get_fps() const
{
    return GetFPS();
}

void RenderManager::draw_text(const char *text, int posX, int posY, int fontSize, Color color) const
{
    DrawText(text, posX, posY, fontSize, color);
}

bool RenderManager::is_window_ready() const
{
    return IsWindowReady();
}

bool RenderManager::window_should_close() const
{
    return WindowShouldClose();
}
