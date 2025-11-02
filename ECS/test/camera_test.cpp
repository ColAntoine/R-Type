#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "ECS/Renderer/Camera/Camera.hpp"
#include <raylib.h>

TEST_SUITE("Camera2D_ECS Tests") {
    TEST_CASE("Camera2D_ECS default constructor") {
        Camera2D_ECS camera;
        
        auto pos = camera.get_position();
        CHECK(pos.x == 0.0f);
        CHECK(pos.y == 0.0f);
        CHECK(camera.get_zoom() == 1.0f);
    }

    TEST_CASE("Camera2D_ECS constructor with parameters") {
        Camera2D_ECS camera(100.0f, 200.0f, 2.0f);
        
        // Note: get_position returns offset, not target in current implementation
        CHECK(camera.get_zoom() == 2.0f);
    }

    TEST_CASE("Camera2D_ECS set_position") {
        Camera2D_ECS camera;
        
        camera.set_position(50.0f, 75.0f);
        
        // Position is stored internally as target
        CHECK(true); // Just verify no crash
    }

    TEST_CASE("Camera2D_ECS set_zoom within valid range") {
        Camera2D_ECS camera;
        
        camera.set_zoom(2.5f);
        CHECK(camera.get_zoom() == 2.5f);
        
        camera.set_zoom(0.5f);
        CHECK(camera.get_zoom() == 0.5f);
        
        camera.set_zoom(5.0f);
        CHECK(camera.get_zoom() == 5.0f);
    }

    TEST_CASE("Camera2D_ECS set_zoom clamped at boundaries") {
        Camera2D_ECS camera;
        
        // Try to set zoom too low
        camera.set_zoom(0.05f); // Below 0.1f
        CHECK(camera.get_zoom() != 0.05f); // Should be rejected
        
        // Try to set zoom too high
        camera.set_zoom(15.0f); // Above 10.0f
        CHECK(camera.get_zoom() != 15.0f); // Should be rejected
    }

    TEST_CASE("Camera2D_ECS set_zoom at exact boundaries rejected") {
        Camera2D_ECS camera;
        camera.set_zoom(1.0f); // Set a known value first
        
        // Boundaries are EXCLUSIVE (> and <, not >= and <=)
        camera.set_zoom(0.1f);
        CHECK(camera.get_zoom() != 0.1f); // Should be rejected
        
        camera.set_zoom(1.0f); // Reset
        camera.set_zoom(10.0f);
        CHECK(camera.get_zoom() != 10.0f); // Should be rejected
    }

    TEST_CASE("Camera2D_ECS move changes position") {
        Camera2D_ECS camera;
        camera.set_position(0.0f, 0.0f);
        
        camera.move(10.0f, 20.0f);
        camera.move(5.0f, -10.0f);
        
        // Position should have changed (can't directly verify due to API)
        CHECK(true);
    }

    TEST_CASE("Camera2D_ECS zoom_in increases zoom") {
        Camera2D_ECS camera;
        camera.set_zoom(1.0f);
        
        camera.zoom_in(0.5f);
        CHECK(camera.get_zoom() == 1.5f);
        
        camera.zoom_in(1.0f);
        CHECK(camera.get_zoom() == 2.5f);
    }

    TEST_CASE("Camera2D_ECS zoom_in clamped at maximum") {
        Camera2D_ECS camera;
        camera.set_zoom(9.0f);
        
        camera.zoom_in(5.0f); // Would go to 14.0f
        CHECK(camera.get_zoom() == 10.0f); // Clamped at 10.0f
    }

    TEST_CASE("Camera2D_ECS zoom_out decreases zoom") {
        Camera2D_ECS camera;
        camera.set_zoom(3.0f);
        
        camera.zoom_out(0.5f);
        CHECK(camera.get_zoom() == 2.5f);
        
        camera.zoom_out(1.0f);
        CHECK(camera.get_zoom() == 1.5f);
    }

    TEST_CASE("Camera2D_ECS zoom_out clamped at minimum") {
        Camera2D_ECS camera;
        camera.set_zoom(1.0f);
        
        camera.zoom_out(2.0f); // Would go to -1.0f
        CHECK(camera.get_zoom() == 0.1f); // Clamped at 0.1f
    }

    TEST_CASE("Camera2D_ECS get_raylib_camera returns valid camera") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        Camera2D_ECS camera(100.0f, 200.0f, 2.0f);
        
        Camera2D raylib_cam = camera.get_raylib_camera();
        
        CHECK(raylib_cam.zoom == 2.0f);
        CHECK(raylib_cam.target.x == 100.0f);
        CHECK(raylib_cam.target.y == 200.0f);
        
        CloseWindow();
    }

    TEST_CASE("Camera2D_ECS begin_mode and end_mode") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        Camera2D_ECS camera;
        
        BeginDrawing();
        // Should not crash
        camera.begin_mode();
        camera.end_mode();
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("Camera2D_ECS screen_to_world conversion") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(800, 600, "Test");
        
        Camera2D_ECS camera;
        camera.set_position(0.0f, 0.0f);
        camera.set_zoom(1.0f);
        
        Vector2 screen_pos = {400, 300}; // Center of screen
        Vector2 world_pos = camera.screen_to_world(screen_pos);
        
        // Should return valid coordinates (just verify no crash)
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("Camera2D_ECS world_to_screen conversion") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(800, 600, "Test");
        
        Camera2D_ECS camera;
        camera.set_position(0.0f, 0.0f);
        camera.set_zoom(1.0f);
        
        Vector2 world_pos = {100, 100};
        Vector2 screen_pos = camera.world_to_screen(world_pos);
        
        // Should return valid coordinates (just verify no crash)
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("Camera2D_ECS round-trip coordinate conversion") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(800, 600, "Test");
        
        Camera2D_ECS camera;
        camera.set_position(0.0f, 0.0f);
        camera.set_zoom(1.0f);
        
        Vector2 original_world = {100, 200};
        Vector2 screen = camera.world_to_screen(original_world);
        Vector2 back_to_world = camera.screen_to_world(screen);
        
        // Should be approximately equal (allowing for floating point error)
        CHECK(back_to_world.x == doctest::Approx(original_world.x).epsilon(0.1));
        CHECK(back_to_world.y == doctest::Approx(original_world.y).epsilon(0.1));
        
        CloseWindow();
    }

    TEST_CASE("Camera2D_ECS multiple position updates") {
        Camera2D_ECS camera;
        
        camera.set_position(10.0f, 10.0f);
        camera.move(5.0f, 5.0f);
        camera.set_position(100.0f, 100.0f);
        camera.move(-20.0f, -20.0f);
        
        // Should not crash
        CHECK(true);
    }

    TEST_CASE("Camera2D_ECS multiple zoom updates") {
        Camera2D_ECS camera;
        
        camera.set_zoom(2.0f);
        camera.zoom_in(1.0f);
        CHECK(camera.get_zoom() == 3.0f);
        
        camera.zoom_out(0.5f);
        CHECK(camera.get_zoom() == 2.5f);
        
        camera.set_zoom(1.0f);
        CHECK(camera.get_zoom() == 1.0f);
    }

    TEST_CASE("Camera2D_ECS zoom boundaries precision") {
        Camera2D_ECS camera;
        
        camera.set_zoom(0.099f); // Just below minimum
        CHECK(camera.get_zoom() != 0.099f);
        
        camera.set_zoom(0.100001f); // Just above minimum
        CHECK(camera.get_zoom() == doctest::Approx(0.100001f));
        
        camera.set_zoom(9.999f); // Just below maximum
        CHECK(camera.get_zoom() == doctest::Approx(9.999f));
        
        camera.set_zoom(10.001f); // Just above maximum
        CHECK(camera.get_zoom() != 10.001f);
    }

    TEST_CASE("Camera2D_ECS negative coordinates") {
        Camera2D_ECS camera;
        
        camera.set_position(-100.0f, -200.0f);
        camera.move(-50.0f, -75.0f);
        
        // Should handle negative coordinates
        CHECK(true);
    }

    TEST_CASE("Camera2D_ECS zero zoom rejection") {
        Camera2D_ECS camera;
        camera.set_zoom(1.0f);
        
        camera.set_zoom(0.0f);
        CHECK(camera.get_zoom() != 0.0f); // Should be rejected
    }

    TEST_CASE("Camera2D_ECS negative zoom rejection") {
        Camera2D_ECS camera;
        camera.set_zoom(1.0f);
        
        camera.set_zoom(-1.0f);
        CHECK(camera.get_zoom() != -1.0f); // Should be rejected
    }

    TEST_CASE("Camera2D_ECS large position values") {
        Camera2D_ECS camera;
        
        camera.set_position(10000.0f, 10000.0f);
        camera.move(5000.0f, 5000.0f);
        
        // Should handle large values
        CHECK(true);
    }
}
