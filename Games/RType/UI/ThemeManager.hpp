#pragma once

#include "ColorPalette.hpp"
#include <string>

enum class ColorBlindMode {
    Normal,
    Protanopia,
    Deuteranopia,
    Tritanopia,
    Protanomaly,
    Deuteranomaly,
    Tritanomaly,
    Achromatopsia,
    Monochromacy
};

class ThemeManager {
public:
    static ThemeManager& instance() {
        static ThemeManager instance;
        return instance;
    }

    void setTheme(const ColorPalette& palette, ColorBlindMode mode = ColorBlindMode::Normal);
    const ColorPalette& getTheme() const;
    const std::string getThemeName() const;

    void setColorBlindMode(ColorBlindMode mode);

private:
    ThemeManager() = default;

    ColorPalette _original{DefaultPalette};
    ColorPalette _current{DefaultPalette};
    ColorBlindMode _mode = ColorBlindMode::Normal;

    static Color applyColorFilter(const Color& c, ColorBlindMode mode);
    static ColorPalette applyColorBlindPalette(const ColorPalette& palette, ColorBlindMode mode);
};
