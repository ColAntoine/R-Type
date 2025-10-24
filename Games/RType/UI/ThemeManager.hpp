#pragma once

#include "ColorPalette.hpp"

class ThemeManager {
    public:
        static ThemeManager& instance() {
            static ThemeManager instance;
            return instance;
        }

        void setTheme(const ColorPalette& palette);
        const ColorPalette& getTheme() const;

    private:
        ThemeManager() = default;
        ColorPalette _current{DefaultPalette};
};
