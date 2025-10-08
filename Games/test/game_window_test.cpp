#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

extern "C" {
    #include "raylib.h"   // pour les FLAGS/KEY_*
}

#include "Core/Window/GameWindow.hpp"
#include <string>

TEST_CASE("GameWindow initialize / draw / close")
{
    // on évite d’ouvrir une vraie fenêtre pendant les tests
    SetConfigFlags(FLAG_WINDOW_HIDDEN);

    GameWindow win(320, 240, "test-window");
    CHECK(win.initialize() == true);
    CHECK(win.is_initialized() == true);
    CHECK(win.get_width() == 320);
    CHECK(win.get_height() == 240);
    CHECK(win.should_window_close() == false);

    // Quelques appels de rendu (ne doivent pas crasher)
    win.begin_drawing();
    win.clear_background(BLACK);
    win.draw_text("hello", 10, 10, 20, RAYWHITE);
    win.draw_rectangle(0, 0, 50, 30, RED);
    win.draw_circle(25, 15, 10.0f, BLUE);
    win.end_drawing();

    // Laisser passer quelques frames pour que FPS/frametime soient > 0
    for (int i = 0; i < 3; ++i) {
        win.begin_drawing();
        win.end_drawing();
    }

    CHECK(win.get_frame_time() >= 0.0f);
    CHECK(win.get_fps() >= 0);

    win.close();
    CHECK(win.is_initialized() == false);
}

TEST_CASE("GameWindow: appels sûrs quand non initialisé")
{
    GameWindow w(100, 80, "no-init");
    CHECK_FALSE(w.is_initialized());
    CHECK_FALSE(w.should_window_close());

    // Les méthodes doivent être no-op et ne pas crasher
    w.begin_drawing();
    w.clear_background(BLACK);
    w.draw_text("text", 0, 0, 10, WHITE);
    w.draw_rectangle(0, 0, 1, 1, WHITE);
    w.draw_circle(1, 1, 1.0f, WHITE);
    w.end_drawing();

    // Les queries d’input renvoient false si pas initialisé
    CHECK_FALSE(w.is_key_pressed(KEY_SPACE));
    CHECK_FALSE(w.is_key_down(KEY_RIGHT));

    // Timings neutres si pas initialisé
    CHECK(w.get_frame_time() == 0.0f);
    CHECK(w.get_fps() == 0);
}
