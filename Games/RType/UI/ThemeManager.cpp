#include "ThemeManager.hpp"
#include <cstdint>

void ThemeManager::setTheme(const ColorPalette& palette, ColorBlindMode mode)
{
    _mode = mode;
    _original = palette;
    _current = applyColorBlindPalette(_original, _mode);
}

const ColorPalette& ThemeManager::getTheme() const
{
    return _current;
}

const std::string ThemeManager::getThemeName() const
{
    return _current.name;
}

void ThemeManager::setColorBlindMode(ColorBlindMode mode)
{
    _mode = mode;
    _current = applyColorBlindPalette(_original, _mode);
}

Color ThemeManager::applyColorFilter(const Color& c, ColorBlindMode mode) {
    float r = c.r;
    float g = c.g;
    float b = c.b;
    uint8_t newR, newG, newB;

    switch (mode) {
        case ColorBlindMode::Normal:
            return c;

        case ColorBlindMode::Protanopia:
            newR = static_cast<uint8_t>(r * 0.567f + g * 0.433f + b * 0.0f);
            newG = static_cast<uint8_t>(r * 0.558f + g * 0.442f + b * 0.0f);
            newB = static_cast<uint8_t>(r * 0.0f   + g * 0.242f + b * 0.758f);
            break;

        case ColorBlindMode::Deuteranopia:
            newR = static_cast<uint8_t>(r * 0.625f + g * 0.375f + b * 0.0f);
            newG = static_cast<uint8_t>(r * 0.7f   + g * 0.3f   + b * 0.0f);
            newB = static_cast<uint8_t>(r * 0.0f   + g * 0.3f   + b * 0.7f);
            break;

        case ColorBlindMode::Tritanopia:
            newR = static_cast<uint8_t>(r * 0.95f  + g * 0.05f  + b * 0.0f);
            newG = static_cast<uint8_t>(r * 0.0f   + g * 0.433f + b * 0.567f);
            newB = static_cast<uint8_t>(r * 0.0f   + g * 0.475f + b * 0.525f);
            break;

        case ColorBlindMode::Protanomaly:
            newR = static_cast<uint8_t>(r * 0.817f + g * 0.183f + b * 0.0f);
            newG = static_cast<uint8_t>(r * 0.333f + g * 0.667f + b * 0.0f);
            newB = static_cast<uint8_t>(r * 0.0f   + g * 0.125f + b * 0.875f);
            break;

        case ColorBlindMode::Deuteranomaly:
            newR = static_cast<uint8_t>(r * 0.8f + g * 0.2f + b * 0.0f);
            newG = static_cast<uint8_t>(r * 0.258f + g * 0.742f + b * 0.0f);
            newB = static_cast<uint8_t>(r * 0.0f + g * 0.142f + b * 0.858f);
            break;

        case ColorBlindMode::Tritanomaly:
            newR = static_cast<uint8_t>(r * 0.967f + g * 0.033f + b * 0.0f);
            newG = static_cast<uint8_t>(r * 0.0f + g * 0.733f + b * 0.267f);
            newB = static_cast<uint8_t>(r * 0.0f + g * 0.183f + b * 0.817f);
            break;

        case ColorBlindMode::Achromatopsia:
            {
                uint8_t gray = static_cast<uint8_t>(0.299f*r + 0.587f*g + 0.114f*b);
                return Color{gray, gray, gray, c.a};
            }

        case ColorBlindMode::Monochromacy:
            {
                uint8_t gray = static_cast<uint8_t>(0.2126f*r + 0.7152f*g + 0.0722f*b);
                return Color{gray, gray, gray, c.a};
            }
    }
    return Color{newR, newG, newB, c.a};
}


ColorPalette ThemeManager::applyColorBlindPalette(const ColorPalette& palette, ColorBlindMode mode)
{
    ColorPalette result = palette;
    result.background = applyColorFilter(result.background, mode);
    result.panelColor = applyColorFilter(result.panelColor, mode);
    result.panelBorderColor = applyColorFilter(result.panelBorderColor, mode);
    result.secondaryTextColor = applyColorFilter(result.secondaryTextColor, mode);
    result.textColor = applyColorFilter(result.textColor, mode);
    result.textBackgroundColor = applyColorFilter(result.textBackgroundColor, mode);

    result.buttonColors.normal = applyColorFilter(result.buttonColors.normal, mode);
    result.buttonColors.border = applyColorFilter(result.buttonColors.border, mode);
    result.buttonColors.neonColor = applyColorFilter(result.buttonColors.neonColor, mode);
    result.buttonColors.neonGlowColor = applyColorFilter(result.buttonColors.neonGlowColor, mode);

    result.exitButtonColors.normal = applyColorFilter(result.exitButtonColors.normal, mode);
    result.exitButtonColors.border = applyColorFilter(result.exitButtonColors.border, mode);
    result.exitButtonColors.neonColor = applyColorFilter(result.exitButtonColors.neonColor, mode);
    result.exitButtonColors.neonGlowColor = applyColorFilter(result.exitButtonColors.neonGlowColor, mode);

    result.gameColors.warning = applyColorFilter(result.gameColors.warning, mode);
    result.gameColors.error = applyColorFilter(result.gameColors.error, mode);
    result.gameColors.success = applyColorFilter(result.gameColors.success, mode);
    result.gameColors.info = applyColorFilter(result.gameColors.info, mode);
    result.gameColors.primary = applyColorFilter(result.gameColors.primary, mode);
    result.gameColors.secondary = applyColorFilter(result.gameColors.secondary, mode);

    return result;
}
