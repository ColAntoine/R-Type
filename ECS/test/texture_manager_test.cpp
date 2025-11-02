#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

// Must be before raylib.h to avoid Windows GDI conflicts
#define NOGDI
#define NOMINMAX

#include "ECS/AssetManager/Texture/TextureManager.hpp"
#include <raylib.h>
#include <filesystem>

// Helper: Create a real 1x1 PNG texture using Raylib
static void create_test_texture(const std::string& path) {
    Image img = GenImageColor(1, 1, RED);
    ExportImage(img, path.c_str());
    UnloadImage(img);
}

TEST_SUITE("TextureManager Tests") {
    TEST_CASE("TextureManager initializes with zero textures") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        TextureManager manager;
        CHECK(manager.count() == 0);
        
        CloseWindow();
    }

    TEST_CASE("TextureManager loads texture successfully") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("test_tex.png");
        
        TextureManager manager;
        Texture2D* tex = manager.load("test_tex.png");
        
        REQUIRE(tex != nullptr);
        CHECK(tex->id > 0);
        CHECK(manager.count() == 1);
        
        manager.clear();
        std::filesystem::remove("test_tex.png");
        CloseWindow();
    }

    TEST_CASE("TextureManager caches textures") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("cached_tex.png");
        
        TextureManager manager;
        Texture2D* tex1 = manager.load("cached_tex.png");
        Texture2D* tex2 = manager.load("cached_tex.png");
        
        REQUIRE(tex1 != nullptr);
        REQUIRE(tex2 != nullptr);
        CHECK(tex1 == tex2); // Same pointer
        CHECK(manager.count() == 1); // Only 1 texture
        
        manager.clear();
        std::filesystem::remove("cached_tex.png");
        CloseWindow();
    }

    TEST_CASE("TextureManager loads multiple different textures") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("tex1.png");
        create_test_texture("tex2.png");
        create_test_texture("tex3.png");
        
        TextureManager manager;
        Texture2D* tex1 = manager.load("tex1.png");
        Texture2D* tex2 = manager.load("tex2.png");
        Texture2D* tex3 = manager.load("tex3.png");
        
        REQUIRE(tex1 != nullptr);
        REQUIRE(tex2 != nullptr);
        REQUIRE(tex3 != nullptr);
        CHECK(manager.count() == 3);
        
        // All should be different pointers
        CHECK(tex1 != tex2);
        CHECK(tex2 != tex3);
        CHECK(tex1 != tex3);
        
        manager.clear();
        std::filesystem::remove("tex1.png");
        std::filesystem::remove("tex2.png");
        std::filesystem::remove("tex3.png");
        CloseWindow();
    }

    TEST_CASE("TextureManager unloads texture correctly") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("unload_tex.png");
        
        TextureManager manager;
        manager.load("unload_tex.png");
        CHECK(manager.count() == 1);
        
        manager.unload("unload_tex.png");
        CHECK(manager.count() == 0);
        
        manager.clear();
        std::filesystem::remove("unload_tex.png");
        CloseWindow();
    }

    TEST_CASE("TextureManager unload non-existent texture") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        TextureManager manager;
        
        // Should not crash
        manager.unload("nonexistent.png");
        CHECK(manager.count() == 0);
        
        CloseWindow();
    }

    TEST_CASE("TextureManager clear removes all textures") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("clear1.png");
        create_test_texture("clear2.png");
        
        TextureManager manager;
        manager.load("clear1.png");
        manager.load("clear2.png");
        CHECK(manager.count() == 2);
        
        manager.clear();
        CHECK(manager.count() == 0);
        
        std::filesystem::remove("clear1.png");
        std::filesystem::remove("clear2.png");
        CloseWindow();
    }

    TEST_CASE("TextureManager reload after unload") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("reload_tex.png");
        
        TextureManager manager;
        Texture2D* tex1 = manager.load("reload_tex.png");
        REQUIRE(tex1 != nullptr);
        
        manager.unload("reload_tex.png");
        CHECK(manager.count() == 0);
        
        Texture2D* tex2 = manager.load("reload_tex.png");
        REQUIRE(tex2 != nullptr);
        CHECK(manager.count() == 1);
        
        manager.clear();
        std::filesystem::remove("reload_tex.png");
        CloseWindow();
    }

    TEST_CASE("TextureManager load missing file returns nullptr") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        TextureManager manager;
        Texture2D* tex = manager.load("does_not_exist.png");
        
        CHECK(tex == nullptr);
        CHECK(manager.count() == 0);
        
        CloseWindow();
    }

    TEST_CASE("TextureManager count accuracy") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("count1.png");
        create_test_texture("count2.png");
        
        TextureManager manager;
        CHECK(manager.count() == 0);
        
        manager.load("count1.png");
        CHECK(manager.count() == 1);
        
        manager.load("count1.png"); // Load same again
        CHECK(manager.count() == 1); // Still 1 (cached)
        
        manager.load("count2.png");
        CHECK(manager.count() == 2);
        
        manager.unload("count1.png");
        CHECK(manager.count() == 1);
        
        manager.unload("count2.png");
        CHECK(manager.count() == 0);
        
        std::filesystem::remove("count1.png");
        std::filesystem::remove("count2.png");
        CloseWindow();
    }

    TEST_CASE("TextureManager handles many textures") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        TextureManager manager;
        
        // Create and load 20 textures
        for (int i = 0; i < 20; i++) {
            std::string filename = "many_tex_" + std::to_string(i) + ".png";
            create_test_texture(filename);
            Texture2D* tex = manager.load(filename);
            CHECK(tex != nullptr);
        }
        
        CHECK(manager.count() == 20);
        
        // Clear all
        manager.clear();
        CHECK(manager.count() == 0);
        
        // Cleanup files
        for (int i = 0; i < 20; i++) {
            std::string filename = "many_tex_" + std::to_string(i) + ".png";
            std::filesystem::remove(filename);
        }
        
        CloseWindow();
    }

    TEST_CASE("TextureManager multiple clear calls") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("multi_clear.png");
        
        TextureManager manager;
        manager.load("multi_clear.png");
        
        manager.clear();
        CHECK(manager.count() == 0);
        
        // Clear again (should not crash)
        manager.clear();
        CHECK(manager.count() == 0);
        
        std::filesystem::remove("multi_clear.png");
        CloseWindow();
    }

    TEST_CASE("TextureManager mixed load/unload operations") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("mix1.png");
        create_test_texture("mix2.png");
        create_test_texture("mix3.png");
        
        TextureManager manager;
        
        manager.load("mix1.png");
        CHECK(manager.count() == 1);
        
        manager.load("mix2.png");
        CHECK(manager.count() == 2);
        
        manager.unload("mix1.png");
        CHECK(manager.count() == 1);
        
        manager.load("mix3.png");
        CHECK(manager.count() == 2);
        
        manager.load("mix1.png"); // Reload mix1
        CHECK(manager.count() == 3);
        
        manager.clear();
        CHECK(manager.count() == 0);
        
        std::filesystem::remove("mix1.png");
        std::filesystem::remove("mix2.png");
        std::filesystem::remove("mix3.png");
        CloseWindow();
    }
}
