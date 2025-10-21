#include "MenusBG.hpp"
#include <iostream>
#include <random>
#include <raylib.h>
#include <cmath>

void MenusBackgroundState::enter()
{
    std::cout << "[MenusBackground] Entering state" << std::endl;

    if (!this->_uiRegistry) {
        this->_uiRegistry = std::make_shared<registry>();
    }
    if (!this->_uiSystems) {
        this->_uiSystems = std::make_shared<UI::UISystem>();
    }

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int font_w = std::max(1, this->_asciiFontSize / 2);
    int font_h = this->_asciiFontSize;

    this->_asciiFontSize = 12;
    this->_asciiCols = std::max(10, (sw + font_w - 1) / font_w);
    this->_asciiRows = std::max(8, (sh + font_h - 1) / font_h);
    this->_asciiGrid.assign(this->_asciiRows, std::string(this->_asciiCols, ' '));

    // Initialize ASCII background with random characters
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> char_dist(0, this->_asciiCharset.size() - 1);
    std::uniform_int_distribution<> row_dist(0, this->_asciiRows - 1);
    std::uniform_int_distribution<> col_dist(0, this->_asciiCols - 1);

    for (int i = 0; i < this->_asciiRows * this->_asciiCols / 4; ++i) {
        int r = row_dist(rng);
        int c = col_dist(rng);
        this->_asciiGrid[r][c] = this->_asciiCharset[char_dist(rng)];
    }

    this->_asciiTimer = 0.0f;
    this->_initialized = true;
}

void MenusBackgroundState::exit()
{
    std::cout << "[Menus Background] Exiting state" << std::endl;
    this->cleanup_ui();
    this->_initialized = false;
}

void MenusBackgroundState::pause()
{
    std::cout << "[Menus Background] Pausing state" << std::endl;
}

void MenusBackgroundState::resume()
{
    std::cout << "[Menus Background] Resuming state" << std::endl;
}

void MenusBackgroundState::update(float delta_time)
{
    if (!this->_initialized)
        return;

    // Update UI systems
    if (this->_uiSystems) {
        this->_uiSystems->update(*this->_uiRegistry, delta_time);
    }

    // Animate ASCII background
    this->_asciiTimer += delta_time;
    if (this->_asciiTimer >= this->_asciiInterval) {
        this->_asciiTimer = 0.0f;

        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<> char_dist(0, this->_asciiCharset.size() - 1);
        std::uniform_int_distribution<> row_dist(0, this->_asciiRows - 1);
        std::uniform_int_distribution<> col_dist(0, this->_asciiCols - 1);

        // Change random characters
        int changes = std::max(1, this->_asciiRows * this->_asciiCols / 20);
        for (int i = 0; i < changes; ++i) {
            int r = row_dist(rng);
            int c = col_dist(rng);
            this->_asciiGrid[r][c] = this->_asciiCharset[char_dist(rng)];
        }
    }
}

void MenusBackgroundState::render()
{
    if (!this->_initialized)
        return;

    // Draw ASCII background
    int start_x = 10;
    int start_y = 10;
    Color ascii_color = Color{100, 150, 200, 150};  // Semi-transparent blue

    for (int r = 0; r < this->_asciiRows; ++r) {
        for (int c = 0; c < this->_asciiCols; ++c) {
            char ch = this->_asciiGrid[r][c];
            if (ch == ' ') continue;

            int x = start_x + c * (this->_asciiFontSize / 2);
            int y = start_y + r * this->_asciiFontSize;

            std::string ch_str(1, ch);
            DrawText(ch_str.c_str(), x, y, this->_asciiFontSize, ascii_color);
        }
    }

    // Render UI components via UISystem
    if (this->_uiSystems && this->_uiRegistry) {
        this->_uiSystems->render(*this->_uiRegistry);
    }
}