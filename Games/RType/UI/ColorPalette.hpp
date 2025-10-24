#pragma once

#include "ECS/Renderer/RenderManager.hpp"
#include <string>

struct ButtonColors {
    Color normal;
    Color border;
    Color neonColor;
    Color neonGlowColor;
};

struct ColorPalette {
    std::string name;

    Color background;
    Color panelColor;
    Color panelBorderColor;
    Color textColor;
    Color textBackgroundColor;
    ButtonColors buttonColors;
    ButtonColors exitButtonColors;
};

static const ColorPalette DefaultPalette = {
    .name = "Default",
    .background = BLACK,
    .panelColor = Color{75, 174, 204, 200},
    .panelBorderColor = Color{230, 230, 230, 255},
    .textColor = WHITE,
    .textBackgroundColor = Color{100, 150, 200, 150},
    .buttonColors = {
        .normal = Color{75, 174, 204, 255},
        .border = WHITE,
        .neonColor = Color{0, 229, 255, 255},
        .neonGlowColor = Color{0, 229, 255, 100}
    },
    .exitButtonColors = {
        .normal = Color{194, 27, 27, 255},
        .border = WHITE,
        .neonColor = Color{255, 80, 80, 255},
        .neonGlowColor = Color{255, 50, 50, 100}
    }
};
