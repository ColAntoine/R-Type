/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Neon/Cyberpunk styled button for RType
*/

#pragma once

#include "ECS/UI/Components/Button.hpp"
#include "ECS/UI/UIBuilder.hpp"
#include "ECS/Renderer/RenderManager.hpp"
#include <cmath>
#include <raylib.h>

namespace RType {
    class GlitchButton : public UI::UIButton {
    public:
        GlitchButton(float x, float y, float w, float h, const std::string& text)
            : UIButton(x, y, w, h, text) {
            _hover_seed = rand() % 1000;
        }

        // Class variables (static) - shared across all GlitchButton instances
        static Color default_neon_color;
        static Color default_neon_glow_color;
        static float default_jitter_amplitude;
        static float default_jitter_speed;
        static bool default_enable_glitch;

        void set_neon_colors(Color neon, Color glow) {
            _neon_color = neon;
            _neon_glow_color = glow;
        }

        void set_glitch_params(float amplitude, float speed, bool enabled = true) {
            _hover_jitter_amplitude = amplitude;
            _hover_jitter_speed = speed;
            _enable_glitch_on_hover = enabled;
        }

    protected:
        void drawButtonBackground() const override;
        void drawButtonText() const override;

        private:
            Color _neon_color{0, 229, 255, 255};
            Color _neon_glow_color{0, 229, 255, 100};
            float _hover_jitter_amplitude{2.0f};
            float _hover_jitter_speed{8.0f};
            bool _enable_glitch_on_hover{true};
            int _hover_seed{0};
    };
}

// ==================== UIBuilder Extension for GlitchButton ====================

// Forward declaration of UIBuilder specialization
template<>
class UIBuilder<RType::GlitchButton> : public UIBuilder<UI::UIButton> {
    public:
        UIBuilder() : UIBuilder<UI::UIButton>() {}

        UIBuilder<RType::GlitchButton>& neonColors(Color neon, Color glow) {
            _neonColor = neon;
            _neonGlowColor = glow;
            return *this;
        }

        UIBuilder<RType::GlitchButton>& glitchParams(float amplitude, float speed, bool enabled = true) {
            _jitterAmplitude = amplitude;
            _jitterSpeed = speed;
            _enableGlitch = enabled;
            return *this;
        }

        // Override all inherited methods to return GlitchButton builder type
        UIBuilder<RType::GlitchButton>& at(float x, float y) {
            UIBuilder<UI::UIButton>::at(x, y);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& centered(float yOffset = 0) {
            UIBuilder<UI::UIButton>::centered(yOffset);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& size(float width, float height) {
            UIBuilder<UI::UIButton>::size(width, height);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& text(const std::string& text) {
            UIBuilder<UI::UIButton>::text(text);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& color(Color baseColor, int hoverAmount = 20, int pressAmount = 20) {
            UIBuilder<UI::UIButton>::color(baseColor, hoverAmount, pressAmount);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& textColor(Color color) {
            UIBuilder<UI::UIButton>::textColor(color);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& fontSize(int size) {
            UIBuilder<UI::UIButton>::fontSize(size);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& border(float thickness, Color color) {
            UIBuilder<UI::UIButton>::border(thickness, color);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& onClick(std::function<void()> callback) {
            UIBuilder<UI::UIButton>::onClick(callback);
            return *this;
        }

        UIBuilder<RType::GlitchButton>& red() {
            UIBuilder<UI::UIButton>::red();
            return *this;
        }

        std::shared_ptr<RType::GlitchButton> build(float screenWidth, float screenHeight) {
            auto button = std::make_shared<RType::GlitchButton>(
                calculateX(screenWidth), calculateY(screenHeight), _width, _height, _text
            );

            UI::ButtonStyle style;
            style.setNormalColor(_normalColor);
            style.setHoveredColor(_hoveredColor);
            style.setPressedColor(_pressedColor);
            style.setTextColor(_textColor);
            style.setFontSize(_fontSize);
            style.setBorderThickness(_borderThickness);
            style.setBorderColor(_borderColor);
            button->setStyle(style);

            if (_onClick) {
                button->setOnClick(_onClick);
            }

            button->set_neon_colors(_neonColor, _neonGlowColor);
            button->set_glitch_params(_jitterAmplitude, _jitterSpeed, _enableGlitch);

            return button;
        }

    private:
        Color _neonColor{RType::GlitchButton::default_neon_color};
        Color _neonGlowColor{RType::GlitchButton::default_neon_glow_color};
        float _jitterAmplitude{RType::GlitchButton::default_jitter_amplitude};
        float _jitterSpeed{RType::GlitchButton::default_jitter_speed};
        bool _enableGlitch{RType::GlitchButton::default_enable_glitch};
};

using GlitchButtonBuilder = UIBuilder<RType::GlitchButton>;