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
    init(title, 1.0f, true);
}

void RenderManager::init(const char *title, float scale, bool fullscreen)
{
    int monitor = 0;

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

        // Initialize window with fullscreen and resizable flags
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(100, 100, title);

        // Disable ESC key from closing the window by default
        SetExitKey(KEY_NULL);

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

        _monitorHeight = _winInfos.getHeight();
        _monitorWidth = _winInfos.getWidth();

        if (_winInfos.getWidth() > 0 && _winInfos.getHeight() > 0) {
            this->_winInfos.setHeight(_winInfos.getHeight() * scale);
            this->_winInfos.setWidth(_winInfos.getWidth() * scale);

            SetWindowSize(_winInfos.getWidth(), _winInfos.getHeight());

            Vector2 monitorPos = GetMonitorPosition(monitor);
            int windowX = monitorPos.x + (GetMonitorWidth(monitor) - _winInfos.getWidth()) / 2;
            int windowY = monitorPos.y + (GetMonitorHeight(monitor) - _winInfos.getHeight()) / 2;
            SetWindowPosition(windowX, windowY);

            SetWindowMinSize(_winInfos.getWidth() / 2, _winInfos.getHeight() / 2);
            SetWindowState(FLAG_WINDOW_RESIZABLE | (fullscreen ? FLAG_FULLSCREEN_MODE : 0));
        }

        if (_winInfos.getFps() > 0) {
            SetTargetFPS(this->_winInfos.getFps());
        }

        std::cout << "RenderManager: Created fullscreen window " << this->_winInfos.getWidth() << "x" << this->_winInfos.getHeight() 
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

void RenderManager::init(const char *title, float scale)
{
    init(title, scale, true);
}

void RenderManager::init(const char *title, bool windowed)
{
    init(title, 1.0f, !windowed);
}

void RenderManager::shutdown() {
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
    if (_font.texture.id != 0) {
        DrawTextEx(_font, text, {static_cast<float>(posX), static_cast<float>(posY)},
                   static_cast<float>(fontSize), 1.0f, color);
    } else {
        DrawText(text, posX, posY, fontSize, color);
    }
}

void RenderManager::draw_text_ex(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint) const {
    DrawTextEx(font, text, position, fontSize, spacing, tint);
}

void RenderManager::draw_circle(int centerX, int centerY, float radius, Color color) {
    DrawCircle(centerX, centerY, radius, color);
}

void RenderManager::draw_rectangle(int posX, int posY, int width, int height, Color color) {
    DrawRectangle(posX, posY, width, height, color);
}

void RenderManager::draw_rectangle_lines_ex(Rectangle rec, float lineThick, Color color) {
    DrawRectangleLinesEx(rec, lineThick, color);
}

void RenderManager::draw_sprite(Texture2D* texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint, int layer) {
    if (!texture || texture->id == 0) return;
    batch_.draw(texture, source, dest, origin, rotation, tint, layer);
}

bool RenderManager::is_window_ready() const
{
    return IsWindowReady();
}

bool RenderManager::window_should_close() const
{
    return WindowShouldClose();
}

bool RenderManager::load_font(const char *fontPath)
{
    Font font = LoadFontEx(fontPath, 128, nullptr, 0);
    if (font.texture.id == 0) {
        std::cerr << "RenderManager: Failed to load font from " << fontPath << std::endl;
        return false;
    }
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    std::cout << "RenderManager: Loaded font from " << fontPath << std::endl;
    _font = font;
    return true;
}

void RenderManager::unload_font()
{
    if (_font.texture.id != 0) {
        UnloadFont(_font);
        std::cout << "RenderManager: Unloaded font" << std::endl;
        _font = Font{};
    }
}

void RenderManager::set_window_size(int width, int height)
{
    SetWindowSize(width, height);

    if (width < _monitorWidth || height < _monitorHeight) {
        SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_UNDECORATED);
        SetWindowPosition(_monitorWidth / 2 - width / 2, _monitorHeight / 2 - height / 2);
    } else {
        SetWindowState(FLAG_FULLSCREEN_MODE);
    }
    _winInfos.setWidth(width);
    _winInfos.setHeight(height);
}
