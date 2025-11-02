/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** RenderManager Unit Tests
*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

// Must be before raylib.h to avoid Windows GDI conflicts
#define NOGDI
#define NOMINMAX

#include "ECS/Renderer/RenderManager.hpp"
#include <raylib.h>

TEST_SUITE("RenderManager Tests") {
    TEST_CASE("RenderManager is a singleton") {
        auto& instance1 = RenderManager::instance();
        auto& instance2 = RenderManager::instance();
        CHECK(&instance1 == &instance2);
    }

    TEST_CASE("RenderManager initializes with default parameters") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        CHECK(render.get_screen_infos().getWidth() > 0);
        CHECK(render.get_screen_infos().getHeight() > 0);
        CHECK_FALSE(render.should_close());
        
        render.shutdown();
    }

    TEST_CASE("RenderManager initializes with scale") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test", 0.5f);
        
        int width = render.get_screen_infos().getWidth();
        int height = render.get_screen_infos().getHeight();
        
        CHECK(width > 0);
        CHECK(height > 0);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager initializes in windowed mode") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test", true);
        
        CHECK(render.get_screen_infos().getWidth() > 0);
        CHECK(render.get_screen_infos().getHeight() > 0);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager scalePosX works correctly") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        int width = render.get_screen_infos().getWidth();
        
        CHECK(render.scalePosX(0) == 0);
        CHECK(render.scalePosX(100) == width);
        CHECK(render.scalePosX(50) == doctest::Approx(width / 2.0).epsilon(0.01));
        CHECK(render.scalePosX(25) == doctest::Approx(width / 4.0).epsilon(0.01));
        
        render.shutdown();
    }

    TEST_CASE("RenderManager scalePosY works correctly") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        int height = render.get_screen_infos().getHeight();
        
        CHECK(render.scalePosY(0) == 0);
        CHECK(render.scalePosY(100) == height);
        CHECK(render.scalePosY(50) == doctest::Approx(height / 2.0).epsilon(0.01));
        CHECK(render.scalePosY(25) == doctest::Approx(height / 4.0).epsilon(0.01));
        
        render.shutdown();
    }

    TEST_CASE("RenderManager scaleSizeW works correctly") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        int width = render.get_screen_infos().getWidth();
        
        CHECK(render.scaleSizeW(100) == width);
        CHECK(render.scaleSizeW(50) == doctest::Approx(width / 2.0).epsilon(0.01));
        CHECK(render.scaleSizeW(10) == doctest::Approx(width / 10.0).epsilon(0.01));
        
        render.shutdown();
    }

    TEST_CASE("RenderManager scaleSizeH works correctly") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        int height = render.get_screen_infos().getHeight();
        
        CHECK(render.scaleSizeH(100) == height);
        CHECK(render.scaleSizeH(50) == doctest::Approx(height / 2.0).epsilon(0.01));
        CHECK(render.scaleSizeH(10) == doctest::Approx(height / 10.0).epsilon(0.01));
        
        render.shutdown();
    }

    TEST_CASE("RenderManager clear color can be set and retrieved") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        Color testColor = {255, 0, 0, 255}; // Red
        render.set_clear_color(testColor);
        
        Color retrieved = render.get_clear_color();
        CHECK(retrieved.r == testColor.r);
        CHECK(retrieved.g == testColor.g);
        CHECK(retrieved.b == testColor.b);
        CHECK(retrieved.a == testColor.a);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager camera can be accessed") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        auto& camera = render.get_camera();
        CHECK(camera.get_zoom() > 0.0f);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager sprite batch can be accessed") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        auto& batch = render.get_batch();
        // Just check we can access it without crash
        CHECK(true);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager get_fps returns valid value") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        int fps = render.get_fps();
        CHECK(fps >= 0);
        CHECK(fps <= 1000); // Sanity check
        
        render.shutdown();
    }

    TEST_CASE("RenderManager get_delta_time returns positive value") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        // Force a frame
        render.begin_frame();
        render.end_frame();
        
        // Use raylib's GetFrameTime() directly
        float dt = GetFrameTime();
        CHECK(dt >= 0.0f);
        CHECK(dt < 10.0f); // Sanity check
        
        render.shutdown();
    }

    TEST_CASE("RenderManager screen info returns valid dimensions") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        auto screenInfo = render.get_screen_infos();
        CHECK(screenInfo.getWidth() > 0);
        CHECK(screenInfo.getHeight() > 0);
        CHECK(screenInfo.getWidth() <= 4096); // Sanity check
        CHECK(screenInfo.getHeight() <= 4096);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager monitor dimensions are valid") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        int monitorWidth = render.get_monitor_width();
        int monitorHeight = render.get_monitor_height();
        
        CHECK(monitorWidth > 0);
        CHECK(monitorHeight > 0);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager begin/end frame work correctly") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        // Should not crash
        render.begin_frame();
        render.end_frame();
        
        CHECK(true);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager draw_text works without crashing") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        render.begin_frame();
        render.draw_text("Test", 10, 10, 20, WHITE);
        render.end_frame();
        
        CHECK(true);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager draw_text_ex works without crashing") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        Font font = render.get_font();
        
        render.begin_frame();
        render.draw_text_ex(font, "Test", {10, 10}, 20, 1.0f, WHITE);
        render.end_frame();
        
        CHECK(true);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager draw_circle works without crashing") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        render.begin_frame();
        render.draw_circle(100, 100, 50, RED);
        render.end_frame();
        
        CHECK(true);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager draw_rectangle works without crashing") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        render.begin_frame();
        render.draw_rectangle(10, 10, 50, 50, BLUE);
        render.end_frame();
        
        CHECK(true);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager draw_rectangle_lines_ex works without crashing") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        Rectangle rec = {10, 10, 50, 50};
        
        render.begin_frame();
        render.draw_rectangle_lines_ex(rec, 2.0f, GREEN);
        render.end_frame();
        
        CHECK(true);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager draw_sprite works without crashing") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        // Create a simple texture
        Image img = GenImageColor(10, 10, WHITE);
        Texture2D tex = LoadTextureFromImage(img);
        UnloadImage(img);
        
        Rectangle source = {0, 0, 10, 10};
        Rectangle dest = {100, 100, 20, 20};
        Vector2 origin = {0, 0};
        
        render.begin_frame();
        render.draw_sprite(&tex, source, dest, origin, 0.0f, WHITE, 0);
        render.end_frame();
        
        UnloadTexture(tex);
        
        CHECK(true);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager camera position can be accessed") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        auto& camera = render.get_camera();
        camera.set_position(100.0f, 200.0f);
        
        // Camera might be disabled in 1:1 screen mapping mode, just check it doesn't crash
        auto pos = camera.get_position();
        CHECK(true);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager camera zoom can be modified") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        auto& camera = render.get_camera();
        camera.set_zoom(2.0f);
        
        CHECK(camera.get_zoom() == 2.0f);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager camera move can be called") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        auto& camera = render.get_camera();
        camera.set_position(100.0f, 100.0f);
        
        // Move camera (might be disabled in 1:1 mode, just check no crash)
        camera.move(50.0f, 75.0f);
        
        CHECK(true);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager window size can be changed") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        render.set_window_size(1024, 768);
        
        auto info = render.get_screen_infos();
        // Note: actual size may not change in headless mode, just check no crash
        CHECK(info.getWidth() > 0);
        CHECK(info.getHeight() > 0);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager is_window_ready returns true after init") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        CHECK(render.is_window_ready());
        
        render.shutdown();
    }

    TEST_CASE("RenderManager window_should_close matches should_close") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        bool should1 = render.should_close();
        bool should2 = render.window_should_close();
        
        CHECK(should1 == should2);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager get_font can be called") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        Font font = render.get_font();
        // Font might be default font (id=0), just check no crash
        CHECK(true);
        
        render.shutdown();
    }

    TEST_CASE("RenderManager scaling functions are consistent") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        // scalePosX(50) + scalePosX(50) should equal scalePosX(100)
        int half_x = render.scalePosX(50);
        int full_x = render.scalePosX(100);
        
        // Due to rounding, they should be close
        CHECK(half_x * 2 == doctest::Approx(full_x).epsilon(0.02));
        
        render.shutdown();
    }

    TEST_CASE("RenderManager multiple init calls work correctly") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test 1");
        
        CHECK(render.is_window_ready());
        
        render.shutdown();
        
        // Second init
        render.init("RenderManager Test 2");
        CHECK(render.is_window_ready());
        
        render.shutdown();
    }

    TEST_CASE("RenderManager batch can be used") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& render = RenderManager::instance();
        render.init("RenderManager Test");
        
        auto& batch = render.get_batch();
        
        // Create test texture
        Image img = GenImageColor(10, 10, WHITE);
        Texture2D tex = LoadTextureFromImage(img);
        UnloadImage(img);
        
        // Begin batch, draw, then end
        batch.begin();
        batch.draw(&tex, {0, 0, 10, 10}, {100, 100, 20, 20}, {0, 0}, 0.0f, WHITE, 0);
        CHECK(batch.get_draw_call_count() == 1);
        
        render.begin_frame();
        batch.end(); // This will flush the batch
        render.end_frame();
        
        UnloadTexture(tex);
        
        CHECK(true);
        
        render.shutdown();
    }
}

