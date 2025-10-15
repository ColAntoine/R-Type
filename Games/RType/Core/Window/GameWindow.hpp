#pragma once

#include "Core/RaylibWrapper.hpp"
#include <string>
#include <atomic>

class GameWindow {
private:
    int width;
    int height;
    std::string title;
    std::atomic<bool> should_close;
    std::atomic<bool> initialized;

public:
    GameWindow(int w = 800, int h = 600, const std::string& window_title = "R-Type Client");
    ~GameWindow();

    bool initialize();
    void close();
    bool is_initialized() const;
    bool should_window_close() const;

    void begin_drawing();
    void end_drawing();
    void clear_background(Color color = RAYWHITE);

    // Basic drawing functions
    void draw_text(const std::string& text, int x, int y, int font_size = 20, Color color = DARKGRAY);
    void draw_rectangle(int x, int y, int width, int height, Color color);
    void draw_circle(int center_x, int center_y, float radius, Color color);

    // Input functions
    bool is_key_pressed(int key);
    bool is_key_down(int key);

    // Window info
    int get_width() const;
    int get_height() const;
    float get_frame_time() const;
    int get_fps() const;
};