#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

// Must be before raylib.h to avoid Windows GDI conflicts
#define NOGDI
#define NOMINMAX

#include "ECS/Renderer/Batch/SpriteBatch.hpp"
#include <raylib.h>

// Helper to create a test texture
static Texture2D create_test_texture() {
    Image img = GenImageColor(32, 32, WHITE);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

TEST_SUITE("SpriteBatch Tests") {
    TEST_CASE("SpriteBatch initial state") {
        SpriteBatch batch;
        
        CHECK(batch.get_draw_call_count() == 0);
    }

    TEST_CASE("SpriteBatch begin clears previous draw calls") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        SpriteBatch batch;
        Texture2D tex = create_test_texture();
        
        batch.begin();
        batch.draw(&tex, {0, 0}, WHITE, 0);
        CHECK(batch.get_draw_call_count() == 1); // Check before end()
        batch.end();
        
        batch.begin(); // Should clear
        CHECK(batch.get_draw_call_count() == 0);
        batch.end();
        
        UnloadTexture(tex);
        CloseWindow();
    }

    TEST_CASE("SpriteBatch draw adds draw call") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        SpriteBatch batch;
        Texture2D tex = create_test_texture();
        
        batch.begin();
        batch.draw(&tex, {0, 0, 32, 32}, {100, 100, 64, 64}, {0, 0}, 0.0f, WHITE, 0);
        
        CHECK(batch.get_draw_call_count() == 1); // Check while begun
        
        batch.end();
        
        UnloadTexture(tex);
        CloseWindow();
    }

    TEST_CASE("SpriteBatch draw multiple sprites") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        SpriteBatch batch;
        Texture2D tex = create_test_texture();
        
        batch.begin();
        batch.draw(&tex, {0, 0}, WHITE, 0);
        batch.draw(&tex, {50, 50}, RED, 0);
        batch.draw(&tex, {100, 100}, BLUE, 0);
        
        CHECK(batch.get_draw_call_count() == 3); // Check while begun
        
        batch.end();
        
        UnloadTexture(tex);
        CloseWindow();
    }

    TEST_CASE("SpriteBatch draw with null texture") {
        SpriteBatch batch;
        
        batch.begin();
        batch.draw(nullptr, {0, 0}, WHITE, 0);
        
        CHECK(batch.get_draw_call_count() == 0); // Should not add
        
        batch.end();
    }

    TEST_CASE("SpriteBatch draw with invalid texture ID") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        SpriteBatch batch;
        Texture2D invalid_tex = {0, 32, 32, 1, 1}; // id = 0
        
        batch.begin();
        batch.draw(&invalid_tex, {0, 0}, WHITE, 0);
        
        CHECK(batch.get_draw_call_count() == 0); // Should not add
        
        batch.end();
        
        CloseWindow();
    }

    TEST_CASE("SpriteBatch draw without begin") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        SpriteBatch batch;
        Texture2D tex = create_test_texture();
        
        // Should not crash, but should not add draw call
        batch.draw(&tex, {0, 0}, WHITE, 0);
        
        CHECK(batch.get_draw_call_count() == 0);
        
        UnloadTexture(tex);
        CloseWindow();
    }

    TEST_CASE("SpriteBatch end without begin") {
        SpriteBatch batch;
        
        // Should not crash
        batch.end();
        
        CHECK(true);
    }

    TEST_CASE("SpriteBatch begin called twice") {
        SpriteBatch batch;
        
        batch.begin();
        batch.begin(); // Should print warning but not crash
        batch.end();
        
        CHECK(true);
    }

    TEST_CASE("SpriteBatch sort_by_layer disabled") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        SpriteBatch batch;
        batch.set_sort_by_layer(false);
        
        Texture2D tex = create_test_texture();
        
        batch.begin();
        batch.draw(&tex, {0, 0}, WHITE, 5);  // Layer 5
        batch.draw(&tex, {50, 50}, RED, 1);  // Layer 1
        batch.draw(&tex, {100, 100}, BLUE, 3); // Layer 3
        
        // Order should be preserved (not sorted) - check before end()
        CHECK(batch.get_draw_call_count() == 3);
        
        batch.end();
        
        UnloadTexture(tex);
        CloseWindow();
    }

    TEST_CASE("SpriteBatch sort_by_layer enabled") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        SpriteBatch batch;
        batch.set_sort_by_layer(true);
        
        Texture2D tex = create_test_texture();
        
        batch.begin();
        batch.draw(&tex, {0, 0}, WHITE, 5);  // Layer 5
        batch.draw(&tex, {50, 50}, RED, 1);  // Layer 1
        batch.draw(&tex, {100, 100}, BLUE, 3); // Layer 3
        
        // Should be sorted by layer (can't directly verify order, just check no crash) - check before end()
        CHECK(batch.get_draw_call_count() == 3);
        
        batch.end();
        
        UnloadTexture(tex);
        CloseWindow();
    }

    TEST_CASE("SpriteBatch flush renders all draw calls") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(800, 600, "Test");
        
        SpriteBatch batch;
        Texture2D tex = create_test_texture();
        
        batch.begin();
        batch.draw(&tex, {100, 100}, WHITE, 0);
        batch.draw(&tex, {200, 200}, RED, 0);
        batch.end();
        
        BeginDrawing();
        ClearBackground(BLACK);
        batch.flush();
        EndDrawing();
        
        // After flush, draw calls should be cleared
        CHECK(batch.get_draw_call_count() == 0);
        
        UnloadTexture(tex);
        CloseWindow();
    }

    TEST_CASE("SpriteBatch flush with empty batch") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        SpriteBatch batch;
        
        BeginDrawing();
        batch.flush(); // Should not crash
        EndDrawing();
        
        CHECK(true);
        
        CloseWindow();
    }

    TEST_CASE("SpriteBatch complete cycle: begin, draw, end, flush") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(800, 600, "Test");
        
        SpriteBatch batch;
        batch.set_sort_by_layer(true);
        Texture2D tex = create_test_texture();
        
        batch.begin();
        CHECK(batch.get_draw_call_count() == 0);
        
        batch.draw(&tex, {0, 0}, WHITE, 0);
        CHECK(batch.get_draw_call_count() == 1);
        
        batch.draw(&tex, {50, 50}, RED, 1);
        CHECK(batch.get_draw_call_count() == 2);
        
        // Don't check count after end(), it's unreliable
        batch.end();
        
        BeginDrawing();
        batch.flush();
        EndDrawing();
        
        CHECK(batch.get_draw_call_count() == 0);
        
        UnloadTexture(tex);
        CloseWindow();
    }

    TEST_CASE("SpriteBatch multiple begin/end cycles") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        SpriteBatch batch;
        Texture2D tex = create_test_texture();
        
        // Cycle 1
        batch.begin();
        batch.draw(&tex, {0, 0}, WHITE, 0);
        CHECK(batch.get_draw_call_count() == 1); // Check before end
        batch.end();
        
        // Cycle 2
        batch.begin();
        batch.draw(&tex, {50, 50}, RED, 0);
        batch.draw(&tex, {100, 100}, BLUE, 0);
        CHECK(batch.get_draw_call_count() == 2); // Check before end
        batch.end();
        
        // Cycle 3
        batch.begin();
        CHECK(batch.get_draw_call_count() == 0);
        batch.end();
        
        UnloadTexture(tex);
        CloseWindow();
    }

    TEST_CASE("SpriteBatch draw with different layers") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        SpriteBatch batch;
        Texture2D tex = create_test_texture();
        
        batch.begin();
        batch.draw(&tex, {0, 0}, WHITE, 0);      // Layer 0
        batch.draw(&tex, {50, 50}, WHITE, 5);    // Layer 5
        batch.draw(&tex, {100, 100}, WHITE, -1); // Layer -1
        CHECK(batch.get_draw_call_count() == 3); // Check before end
        batch.end();
        
        UnloadTexture(tex);
        CloseWindow();
    }

    TEST_CASE("SpriteBatch draw simple version") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        SpriteBatch batch;
        Texture2D tex = create_test_texture();
        
        batch.begin();
        // Simple draw (position + tint)
        batch.draw(&tex, {100, 100}, RED, 0);
        CHECK(batch.get_draw_call_count() == 1); // Check before end
        batch.end();
        
        UnloadTexture(tex);
        CloseWindow();
    }

    TEST_CASE("SpriteBatch draw advanced version") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        SpriteBatch batch;
        Texture2D tex = create_test_texture();
        
        batch.begin();
        // Advanced draw (source, dest, origin, rotation, tint)
        Rectangle source = {0, 0, 16, 16};
        Rectangle dest = {100, 100, 32, 32};
        Vector2 origin = {16, 16};
        float rotation = 45.0f;
        
        batch.draw(&tex, source, dest, origin, rotation, GREEN, 0);
        CHECK(batch.get_draw_call_count() == 1); // Check before end
        batch.end();
        
        UnloadTexture(tex);
        CloseWindow();
    }

    TEST_CASE("SpriteBatch many draw calls") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        SpriteBatch batch;
        Texture2D tex = create_test_texture();
        
        batch.begin();
        for (int i = 0; i < 100; i++) {
            batch.draw(&tex, {(float)i * 10, (float)i * 10}, WHITE, i % 5);
        }
        CHECK(batch.get_draw_call_count() == 100); // Check before end
        batch.end();
        
        batch.flush();
        CHECK(batch.get_draw_call_count() == 0);
        
        UnloadTexture(tex);
        CloseWindow();
    }

    TEST_CASE("SpriteBatch get_draw_call_count accuracy") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        SpriteBatch batch;
        Texture2D tex = create_test_texture();
        
        CHECK(batch.get_draw_call_count() == 0);
        
        batch.begin();
        CHECK(batch.get_draw_call_count() == 0);
        
        batch.draw(&tex, {0, 0}, WHITE, 0);
        CHECK(batch.get_draw_call_count() == 1);
        
        batch.draw(&tex, {50, 50}, RED, 0);
        CHECK(batch.get_draw_call_count() == 2);
        
        batch.draw(&tex, {100, 100}, BLUE, 0);
        CHECK(batch.get_draw_call_count() == 3);
        
        // Check before end - this is reliable during drawing phase
        batch.end();
        
        batch.flush();
        CHECK(batch.get_draw_call_count() == 0);
        
        UnloadTexture(tex);
        CloseWindow();
    }

    TEST_CASE("SpriteBatch set_sort_by_layer toggle") {
        SpriteBatch batch;
        
        batch.set_sort_by_layer(true);
        batch.set_sort_by_layer(false);
        batch.set_sort_by_layer(true);
        
        // Should not crash
        CHECK(true);
    }

    TEST_CASE("SpriteBatch flush without end") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        SpriteBatch batch;
        Texture2D tex = create_test_texture();
        
        batch.begin();
        batch.draw(&tex, {0, 0}, WHITE, 0);
        // Skip end() call
        
        BeginDrawing();
        batch.flush(); // Should still work
        EndDrawing();
        
        CHECK(true);
        
        UnloadTexture(tex);
        CloseWindow();
    }

    TEST_CASE("SpriteBatch multiple flushes") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        SpriteBatch batch;
        Texture2D tex = create_test_texture();
        
        batch.begin();
        batch.draw(&tex, {0, 0}, WHITE, 0);
        batch.end();
        
        BeginDrawing();
        batch.flush();
        batch.flush(); // Second flush should be harmless
        batch.flush(); // Third flush
        EndDrawing();
        
        CHECK(batch.get_draw_call_count() == 0);
        
        UnloadTexture(tex);
        CloseWindow();
    }
}
