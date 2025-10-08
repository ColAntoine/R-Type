#include "GameWindow.hpp"
#include <iostream>

GameWindow::GameWindow(int w, int h, const std::string& window_title)
    : width(w), height(h), title(window_title), should_close(false), initialized(false) {
}

GameWindow::~GameWindow() {
    close();
}

bool GameWindow::initialize() {
    if (initialized.load()) {
        std::cout << "Window already initialized" << std::endl;
        return true;
    }

    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);

    if (!IsWindowReady()) {
        std::cerr << "Failed to initialize raylib window" << std::endl;
        return false;
    }

    initialized.store(true);
    std::cout << "Raylib window initialized: " << width << "x" << height << std::endl;
    return true;
}

void GameWindow::close() {
    if (initialized.load()) {
        CloseWindow();
        initialized.store(false);
        std::cout << "Raylib window closed" << std::endl;
    }
}

bool GameWindow::is_initialized() const {
    return initialized.load();
}

bool GameWindow::should_window_close() const {
    return initialized.load() && WindowShouldClose();
}

void GameWindow::begin_drawing() {
    if (initialized.load()) {
        BeginDrawing();
    }
}

void GameWindow::end_drawing() {
    if (initialized.load()) {
        EndDrawing();
    }
}

void GameWindow::clear_background(Color color) {
    if (initialized.load()) {
        ClearBackground(color);
    }
}

void GameWindow::draw_text(const std::string& text, int x, int y, int font_size, Color color) {
    if (initialized.load()) {
        DrawText(text.c_str(), x, y, font_size, color);
    }
}

void GameWindow::draw_rectangle(int x, int y, int w, int h, Color color) {
    if (initialized.load()) {
        DrawRectangle(x, y, w, h, color);
    }
}

void GameWindow::draw_circle(int center_x, int center_y, float radius, Color color) {
    if (initialized.load()) {
        DrawCircle(center_x, center_y, radius, color);
    }
}

bool GameWindow::is_key_pressed(int key) {
    return initialized.load() && IsKeyPressed(key);
}

bool GameWindow::is_key_down(int key) {
    return initialized.load() && IsKeyDown(key);
}

int GameWindow::get_width() const {
    return width;
}

int GameWindow::get_height() const {
    return height;
}

float GameWindow::get_frame_time() const {
    return initialized.load() ? GetFrameTime() : 0.0f;
}

int GameWindow::get_fps() const {
    return initialized.load() ? GetFPS() : 0;
}