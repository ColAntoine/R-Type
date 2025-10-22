#include "MenusBG.hpp"
#include "ECS/UI/Components/Text.hpp"
#include <iostream>
#include <random>
#include <raylib.h>
#include <cmath>

void MenusBackgroundState::enter()
{
    std::cout << "[MenusBackground] Entering state" << std::endl;

    setup_ui();
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

    this->_asciiTimer += delta_time;
    if (this->_asciiTimer >= this->_asciiInterval) {
        this->_asciiTimer = 0.0f;

        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<> char_dist(0, static_cast<int>(this->_asciiCharset.size() - 1));
        std::uniform_int_distribution<> row_dist(0, this->_asciiRows - 1);
        std::uniform_int_distribution<> col_dist(0, this->_asciiCols - 1);

        // Change random characters
        int changes = std::max(1, static_cast<int>(this->_asciiRows * this->_asciiCols / 20));
        for (int i = 0; i < changes; ++i) {
            int r = row_dist(rng);
            int c = col_dist(rng);
            this->_asciiGrid[r][c] = this->_asciiCharset[char_dist(rng)];
        }
    }
    this->_uiSystems.update(this->_uiRegistry, delta_time);
}

void MenusBackgroundState::setup_ui()
{
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int font_w = std::max(1, this->_asciiFontSize / 2);
    int font_h = this->_asciiFontSize;

    this->_asciiCols = std::max(10, (sw + font_w - 1) / font_w);
    this->_asciiRows = std::max(8, (sh + font_h - 1) / font_h);
    this->_asciiGrid.assign(this->_asciiRows, std::string(this->_asciiCols, ' '));

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> char_dist(0, static_cast<int>(this->_asciiCharset.size() - 1));
    std::uniform_int_distribution<> row_dist(0, this->_asciiRows - 1);
    std::uniform_int_distribution<> col_dist(0, this->_asciiCols - 1);

    for (int i = 0; i < static_cast<int>(this->_asciiRows * this->_asciiCols / 4); ++i) {
        int r = row_dist(rng);
        int c = col_dist(rng);
        this->_asciiGrid[r][c] = this->_asciiCharset[char_dist(rng)];
    }

    auto ascii_text_entity = this->_uiRegistry.spawn_entity();
    auto ascii_text = std::make_shared<UI::UIText>(0, 0, " ");

    UI::TextStyle ascii_text_style;
    ascii_text_style.setTextColor({100, 150, 200, 150});  // Semi-transparent blue
    ascii_text_style.setFontSize(this->_asciiFontSize);
    ascii_text_style.setAlignment(UI::TextAlignment::Left);
    ascii_text->setStyle(ascii_text_style);

    ascii_text->setCustomRender([this](const UI::UIText& text) {
        Vector2 pos = text.getPosition();
        Color color = text.getStyle().getTextColor();
        int font_size = text.getStyle().getFontSize();
        int font_w = std::max(1, font_size / 2);

        for (int r = 0; r < static_cast<int>(this->_asciiRows); ++r) {
            for (int c = 0; c < static_cast<int>(this->_asciiCols); ++c) {
                char ch = this->_asciiGrid[r][c];
                if (ch == ' ') continue;

                int x = static_cast<int>(pos.x) + c * font_w;
                int y = static_cast<int>(pos.y) + r * font_size;

                std::string ch_str(1, ch);
                DrawText(ch_str.c_str(), x, y, font_size, color);
            }
        }
    });
    this->_uiRegistry.add_component(ascii_text_entity, UI::UIComponent(ascii_text));
    this->_asciiTextEntity = ascii_text_entity;

    this->_asciiTimer = 0.0f;
}
