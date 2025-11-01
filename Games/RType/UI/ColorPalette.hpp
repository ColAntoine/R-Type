#pragma once

#include "ECS/Renderer/RenderManager.hpp"
#include <string>

struct ButtonColors {
    Color normal;
    Color border;
    Color neonColor;
    Color neonGlowColor;
};

struct InputFieldColors {
    Color background;
    Color placeholder;
    Color border;

    Color focused;
    Color focusedBorder;
};

struct GameColors {
    Color warning;
    Color error;
    Color success;
    Color info;

    Color primary;
    Color secondary;
};

struct LobbyStateColors {
    Color ready;
    Color notReady;
};

struct ColorPalette {
    std::string name;

    Color background;
    Color panelColor;
    Color panelBorderColor;
    Color secondaryTextColor;
    Color textColor;
    Color textBackgroundColor;
    ButtonColors buttonColors;
    ButtonColors secondaryButtonColors;
    ButtonColors exitButtonColors;
    GameColors gameColors;
    LobbyStateColors lobbyStateColors;

    InputFieldColors inputFieldColors;
};

static const ColorPalette DefaultPalette = {
    .name = "Default",
    .background = BLACK,
    .panelColor = Color{15, 130, 140, 150},
    .panelBorderColor = Color{230, 230, 230, 150},
    .secondaryTextColor = Color{255, 200, 50, 255},
    .textColor = Color{152, 227, 221, 255},
    .textBackgroundColor = Color{118, 74, 181, 150},
    .buttonColors = {
        .normal = Color{10, 120, 200, 255},
        .border = WHITE,
        .neonColor = Color{0, 180, 255, 255},
        .neonGlowColor = Color{0, 180, 255, 100}
    },
    .secondaryButtonColors = {
        .normal = Color{34, 139, 34, 255},
        .border = WHITE,
        .neonColor = Color{255, 200, 0, 255},
        .neonGlowColor = Color{255, 200, 0, 100}
    },
    .exitButtonColors = {
        .normal = Color{194, 27, 27, 255},
        .border = WHITE,
        .neonColor = Color{255, 80, 80, 255},
        .neonGlowColor = Color{255, 50, 50, 100}
    },
    .gameColors = {
        .warning = ORANGE,
        .error = RED,
        .success = GREEN,
        .info = Color{0, 128, 255, 255},
        .primary = WHITE,
        .secondary = RED
    },
    .lobbyStateColors = {
        .ready = GREEN,
        .notReady = WHITE
    },
    .inputFieldColors = {
        .background = Color{50, 50, 50, 255},
        .placeholder = Color{150, 150, 150, 255},
        .border = Color{200, 200, 200, 255},
        .focused = Color{70, 70, 70, 255},
        .focusedBorder = Color{255, 255, 255, 255}
    }
};
