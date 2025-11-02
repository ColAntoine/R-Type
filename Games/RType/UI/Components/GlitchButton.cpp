/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GlitchButton implementation
*/

#include "GlitchButton.hpp"
#include "ECS/Renderer/RenderManager.hpp"

namespace RType {
    Color GlitchButton::default_neon_color{0, 229, 255, 255};
    Color GlitchButton::default_neon_glow_color{0, 229, 255, 100};
    float GlitchButton::default_jitter_amplitude = 2.0f;
    float GlitchButton::default_jitter_speed = 8.0f;
    bool GlitchButton::default_enable_glitch = true;

    void GlitchButton::drawButtonBackground() const {
        // Call base implementation
        UIButton::drawButtonBackground();

        // Add scanline texture
        for (int i = 0; i < static_cast<int>(_size.y); i += 6) {
            DrawRectangle(_position.x, _position.y + i, _size.x, 1, {0, 0, 0, 10});
        }

        // Add neon glow when hovered
        if (_state == UI::UIState::Hovered) {
            DrawRectangleLinesEx(
                {_position.x - 2, _position.y - 2, _size.x + 4, _size.y + 4},
                2.0f,
                _neon_color
            );
            DrawRectangle(
                _position.x - 4,
                _position.y - 4,
                _size.x + 8,
                4,
                _neon_glow_color
            );
        }

        // Pressed state highlight
        if (_state == UI::UIState::Pressed) {
            DrawRectangle(
                _position.x + 2,
                _position.y + 2,
                _size.x - 4,
                _size.y - 4,
                {255, 255, 255, 30}
            );
        }
    }

    void GlitchButton::drawButtonText() const {
        RenderManager& renderManager = RenderManager::instance();
        Font font = renderManager.get_font();
        Color text_color = _style.getTextColor();
        Vector2 pos = _position;
        Vector2 size = _size;

        Vector2 text_size = MeasureTextEx(font, getText().c_str(), _style.getFontSize(), 1.0f);
        float text_x = pos.x + (size.x - text_size.x) / 2.0f;
        float text_y = pos.y + (size.y - text_size.y) / 2.0f;

        // Glitch effect on hover
        if (_state == UI::UIState::Hovered && _enable_glitch_on_hover) {
            float t = GetTime() * _hover_jitter_speed + _hover_seed;
            float jitter_x = std::sin(t) * _hover_jitter_amplitude;
            float jitter_y = std::cos(t * 1.3f) * (_hover_jitter_amplitude / 2.0f);

            // RGB split effect
            Color rcol = {255, 50, 80, text_color.a};
            DrawTextEx(font, getText().c_str(),
                      {text_x + jitter_x + 1, text_y + jitter_y},
                      _style.getFontSize(), 1.0f, rcol);

            Color gcol = {0, 255, 156, static_cast<unsigned char>(text_color.a * 0.6f)};
            DrawTextEx(font, getText().c_str(),
                      {text_x - jitter_x - 1, text_y - jitter_y},
                      _style.getFontSize(), 1.0f, gcol);

            DrawTextEx(font, getText().c_str(),
                      {text_x, text_y}, _style.getFontSize(), 1.0f, text_color);

            // Scanline flicker
            if (((static_cast<int>(GetTime() * 10) + _hover_seed) % 5) == 0) {
                DrawRectangle(text_x - 4, text_y + text_size.y + 2,
                            text_size.x + 8, 2, {255, 255, 255, 10});
            }
        } else {
            // Normal rendering
            if (_state == UI::UIState::Pressed) {
                text_x += 1;
                text_y += 1;
            }
            DrawTextEx(font, getText().c_str(),
                      {text_x, text_y}, _style.getFontSize(), 1.0f, text_color);
        }
    }
}
