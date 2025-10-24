/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** GlitchButton implementation - static member initialization
*/

#include "GlitchButton.hpp"

namespace RType {
    // Initialize static class variables with default values
    Color GlitchButton::default_neon_color{0, 229, 255, 255};
    Color GlitchButton::default_neon_glow_color{0, 229, 255, 100};
    float GlitchButton::default_jitter_amplitude = 2.0f;
    float GlitchButton::default_jitter_speed = 8.0f;
    bool GlitchButton::default_enable_glitch = true;
}
