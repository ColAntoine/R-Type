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
    float scale = 1.0f; // Useful for testing on smaller screens, in production set to 1.0f

    if (!IsWindowReady()) {
        // Detect display environment and set appropriate flags
        const char* session_type = std::getenv("XDG_SESSION_TYPE");
        const char* wayland_display = std::getenv("WAYLAND_DISPLAY");

        if (wayland_display != nullptr && std::string(wayland_display).length() > 0) {
            // Running on Wayland - prefer Wayland backend over X11
            std::cout << "RenderManager: Detected Wayland session, preferring Wayland backend" << std::endl;
        } else {
            // Running on X11
            std::cout << "RenderManager: Using X11/GLX backend" << std::endl;
        }

        // Initialize window with resizable flag
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(100, 100, title);

        if (!IsWindowReady()) {
            std::cerr << "RenderManager: Failed to create main window" << std::endl;
            std::cerr << "  - Requested resolution: " << this->_winInfos.getWidth() << "x" << this->_winInfos.getHeight() << std::endl;
            std::cerr << "  - Check if display server is available" << std::endl;
            std::cerr << "  - DISPLAY=" << (std::getenv("DISPLAY") != nullptr ? std::getenv("DISPLAY") : "unset") << std::endl;
            std::cerr << "  - WAYLAND_DISPLAY=" << (wayland_display != nullptr ? wayland_display : "unset") << std::endl;
            std::cerr << "  - XDG_SESSION_TYPE=" << (session_type != nullptr ? session_type : "unset") << std::endl;
            return;
        }

        // Now adjust window based on monitor info
        monitor = GetCurrentMonitor();
        _winInfos.setFps(GetMonitorRefreshRate(monitor));
        _winInfos.setHeight(GetMonitorHeight(monitor));
        _winInfos.setWidth(GetMonitorWidth(monitor));

        // Only adjust if monitor values are reasonable (> 0)
        if (_winInfos.getWidth() > 0 && _winInfos.getHeight() > 0) {
            this->_winInfos.setHeight(_winInfos.getHeight() * scale);
            this->_winInfos.setWidth(_winInfos.getWidth() * scale);
            SetWindowMinSize(_winInfos.getWidth(), _winInfos.getHeight());
            SetWindowState(FLAG_WINDOW_RESIZABLE);
        }

        if (_winInfos.getFps() > 0) {
            SetTargetFPS(this->_winInfos.getFps());
        }

        std::cout << "RenderManager: Created window " << this->_winInfos.getWidth() << "x" << this->_winInfos.getHeight() 
                  << " @ " << this->_winInfos.getFps() << "Hz on monitor " << monitor << std::endl;
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
