#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "ECS/AssetManager/AssetManager.hpp"
#include <raylib.h>
#include <filesystem>

// Helper: Create a real 1x1 PNG texture using Raylib
static void create_test_texture(const std::string& path) {
    Image img = GenImageColor(1, 1, RED);
    ExportImage(img, path.c_str());
    UnloadImage(img);
}

TEST_SUITE("AssetManager Tests") {
    TEST_CASE("AssetManager is a singleton") {
        auto& instance1 = AssetManager::instance();
        auto& instance2 = AssetManager::instance();
        CHECK(&instance1 == &instance2);
    }

    TEST_CASE("AssetManager initializes successfully") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        auto& assets = AssetManager::instance();
        assets.init();
        
        CHECK(true);
        
        assets.shutdown();
        CloseWindow();
    }

    TEST_CASE("AssetManager caches textures correctly") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("test_texture.png");
        
        auto& assets = AssetManager::instance();
        assets.init();
        
        SUBCASE("First load creates texture") {
            Texture2D* tex1 = assets.get_texture("test_texture.png");
            REQUIRE(tex1 != nullptr);
            CHECK(tex1->id > 0);
        }
        
        SUBCASE("Second load returns cached texture") {
            Texture2D* tex1 = assets.get_texture("test_texture.png");
            Texture2D* tex2 = assets.get_texture("test_texture.png");
            
            REQUIRE(tex1 != nullptr);
            REQUIRE(tex2 != nullptr);
            CHECK(tex1 == tex2);
            CHECK(assets.texture_count() == 1);
        }
        
        assets.unload_texture("test_texture.png");
        assets.shutdown();
        std::filesystem::remove("test_texture.png");
        CloseWindow();
    }

    // Audio tests are disabled because they crash in headless/CI environments
    // To enable audio tests, compile with: -DENABLE_AUDIO_TESTS
    #ifdef ENABLE_AUDIO_TESTS
    TEST_CASE("AssetManager caches sounds correctly") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        InitAudioDevice();
        
        // Create test WAV
        Wave wave = {
            .frameCount = 44100,
            .sampleRate = 44100,
            .sampleSize = 16,
            .channels = 1,
            .data = calloc(44100 * 2, 1)
        };
        ExportWave(wave, "test_sound.wav");
        UnloadWave(wave);
        
        auto& assets = AssetManager::instance();
        assets.init();
        
        SUBCASE("First load creates sound") {
            Sound* sound1 = assets.get_sound("test_sound.wav");
            REQUIRE(sound1 != nullptr);
            CHECK(true);
        }
        
        SUBCASE("Second load returns cached sound") {
            Sound* sound1 = assets.get_sound("test_sound.wav");
            Sound* sound2 = assets.get_sound("test_sound.wav");
            
            REQUIRE(sound1 != nullptr);
            REQUIRE(sound2 != nullptr);
            CHECK(sound1 == sound2);
            CHECK(assets.sound_count() == 1);
        }
        
        assets.unload_sound("test_sound.wav");
        assets.shutdown();
        std::filesystem::remove("test_sound.wav");
        CloseAudioDevice();
        CloseWindow();
    }
    #else
    TEST_CASE("AssetManager sound cache logic (no audio device)" * doctest::description("Audio tests disabled - compile with -DENABLE_AUDIO_TESTS to enable")) {
        auto& assets = AssetManager::instance();
        assets.init();
        
        // Test cache count without actual audio loading
        CHECK(assets.sound_count() == 0);
        
        // Test that non-existent sound returns nullptr
        Sound* fake = assets.get_sound("nonexistent.wav");
        CHECK(fake == nullptr);
        CHECK(assets.sound_count() == 0);
        
        assets.shutdown();
    }
    #endif

    TEST_CASE("AssetManager handles missing files gracefully") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        auto& assets = AssetManager::instance();
        assets.init();
        
        Texture2D* tex = assets.get_texture("nonexistent.png");
        CHECK(tex == nullptr);
        
        assets.shutdown();
        CloseWindow();
    }

    TEST_CASE("AssetManager unloads resources correctly") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("test_unload.png");
        
        auto& assets = AssetManager::instance();
        assets.init();
        
        Texture2D* tex = assets.get_texture("test_unload.png");
        REQUIRE(tex != nullptr);
        CHECK(assets.texture_count() == 1);
        
        assets.unload_texture("test_unload.png");
        CHECK(assets.texture_count() == 0);
        
        Texture2D* tex2 = assets.get_texture("test_unload.png");
        REQUIRE(tex2 != nullptr);
        CHECK(assets.texture_count() == 1);
        
        assets.unload_texture("test_unload.png");
        assets.shutdown();
        std::filesystem::remove("test_unload.png");
        CloseWindow();
    }

    TEST_CASE("AssetManager tracks resource count correctly") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("test1.png");
        create_test_texture("test2.png");
        
        auto& assets = AssetManager::instance();
        assets.init();
        
        CHECK(assets.texture_count() == 0);
        
        assets.get_texture("test1.png");
        CHECK(assets.texture_count() == 1);
        
        assets.get_texture("test2.png");
        CHECK(assets.texture_count() == 2);
        
        assets.unload_texture("test1.png");
        CHECK(assets.texture_count() == 1);
        
        assets.shutdown();
        std::filesystem::remove("test1.png");
        std::filesystem::remove("test2.png");
        CloseWindow();
    }

    TEST_CASE("AssetManager loads multiple textures simultaneously") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("multi1.png");
        create_test_texture("multi2.png");
        create_test_texture("multi3.png");
        
        auto& assets = AssetManager::instance();
        assets.init();
        
        Texture2D* tex1 = assets.get_texture("multi1.png");
        Texture2D* tex2 = assets.get_texture("multi2.png");
        Texture2D* tex3 = assets.get_texture("multi3.png");
        
        REQUIRE(tex1 != nullptr);
        REQUIRE(tex2 != nullptr);
        REQUIRE(tex3 != nullptr);
        CHECK(assets.texture_count() == 3);
        
        // All textures should be different
        CHECK(tex1 != tex2);
        CHECK(tex2 != tex3);
        CHECK(tex1 != tex3);
        
        assets.shutdown();
        std::filesystem::remove("multi1.png");
        std::filesystem::remove("multi2.png");
        std::filesystem::remove("multi3.png");
        CloseWindow();
    }

    TEST_CASE("AssetManager loads same texture multiple times returns same pointer") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("same.png");
        
        auto& assets = AssetManager::instance();
        assets.init();
        
        Texture2D* tex1 = assets.get_texture("same.png");
        Texture2D* tex2 = assets.get_texture("same.png");
        Texture2D* tex3 = assets.get_texture("same.png");
        
        REQUIRE(tex1 != nullptr);
        REQUIRE(tex2 != nullptr);
        REQUIRE(tex3 != nullptr);
        
        // All pointers should be identical (cached)
        CHECK(tex1 == tex2);
        CHECK(tex2 == tex3);
        CHECK(assets.texture_count() == 1);
        
        assets.shutdown();
        std::filesystem::remove("same.png");
        CloseWindow();
    }

    TEST_CASE("AssetManager shutdown clears all resources") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("clear1.png");
        create_test_texture("clear2.png");
        
        auto& assets = AssetManager::instance();
        assets.init();
        
        assets.get_texture("clear1.png");
        assets.get_texture("clear2.png");
        CHECK(assets.texture_count() == 2);
        
        assets.shutdown();
        CHECK(assets.texture_count() == 0);
        
        // Should be able to reinitialize and use
        assets.init();
        Texture2D* tex = assets.get_texture("clear1.png");
        CHECK(tex != nullptr);
        CHECK(assets.texture_count() == 1);
        
        assets.shutdown();
        std::filesystem::remove("clear1.png");
        std::filesystem::remove("clear2.png");
        CloseWindow();
    }

    TEST_CASE("AssetManager reloads texture after unload") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("reload.png");
        
        auto& assets = AssetManager::instance();
        assets.init();
        
        Texture2D* tex1 = assets.get_texture("reload.png");
        REQUIRE(tex1 != nullptr);
        
        assets.unload_texture("reload.png");
        CHECK(assets.texture_count() == 0);
        
        // Reload the same texture
        Texture2D* tex2 = assets.get_texture("reload.png");
        REQUIRE(tex2 != nullptr);
        CHECK(assets.texture_count() == 1);
        
        // Both should be valid textures (Raylib may reuse IDs, so we just check validity)
        CHECK(tex2->id > 0);
        
        assets.shutdown();
        std::filesystem::remove("reload.png");
        CloseWindow();
    }

    TEST_CASE("AssetManager handles unload of non-existent texture") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        auto& assets = AssetManager::instance();
        assets.init();
        
        // Should not crash
        assets.unload_texture("nonexistent.png");
        CHECK(assets.texture_count() == 0);
        
        assets.shutdown();
        CloseWindow();
    }

    TEST_CASE("AssetManager texture count accuracy with mixed operations") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        
        create_test_texture("mix1.png");
        create_test_texture("mix2.png");
        create_test_texture("mix3.png");
        
        auto& assets = AssetManager::instance();
        assets.init();
        
        CHECK(assets.texture_count() == 0);
        
        assets.get_texture("mix1.png");
        CHECK(assets.texture_count() == 1);
        
        assets.get_texture("mix1.png"); // Same texture
        CHECK(assets.texture_count() == 1);
        
        assets.get_texture("mix2.png");
        CHECK(assets.texture_count() == 2);
        
        assets.unload_texture("mix1.png");
        CHECK(assets.texture_count() == 1);
        
        assets.get_texture("mix3.png");
        CHECK(assets.texture_count() == 2);
        
        assets.unload_texture("mix2.png");
        assets.unload_texture("mix3.png");
        CHECK(assets.texture_count() == 0);
        
        assets.shutdown();
        std::filesystem::remove("mix1.png");
        std::filesystem::remove("mix2.png");
        std::filesystem::remove("mix3.png");
        CloseWindow();
    }

    #ifdef ENABLE_AUDIO_TESTS
    TEST_CASE("AssetManager loads multiple sounds simultaneously") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        InitAudioDevice();
        
        // Create test WAVs
        for (int i = 1; i <= 3; i++) {
            Wave wave = {
                .frameCount = 44100,
                .sampleRate = 44100,
                .sampleSize = 16,
                .channels = 1,
                .data = calloc(44100 * 2, 1)
            };
            ExportWave(wave, ("multi_sound" + std::to_string(i) + ".wav").c_str());
            UnloadWave(wave);
        }
        
        auto& assets = AssetManager::instance();
        assets.init();
        
        Sound* s1 = assets.get_sound("multi_sound1.wav");
        Sound* s2 = assets.get_sound("multi_sound2.wav");
        Sound* s3 = assets.get_sound("multi_sound3.wav");
        
        REQUIRE(s1 != nullptr);
        REQUIRE(s2 != nullptr);
        REQUIRE(s3 != nullptr);
        CHECK(assets.sound_count() == 3);
        
        CHECK(s1 != s2);
        CHECK(s2 != s3);
        
        assets.shutdown();
        std::filesystem::remove("multi_sound1.wav");
        std::filesystem::remove("multi_sound2.wav");
        std::filesystem::remove("multi_sound3.wav");
        CloseAudioDevice();
        CloseWindow();
    }

    TEST_CASE("AssetManager shutdown clears all sounds") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        InitAudioDevice();
        
        Wave wave = {
            .frameCount = 44100,
            .sampleRate = 44100,
            .sampleSize = 16,
            .channels = 1,
            .data = calloc(44100 * 2, 1)
        };
        ExportWave(wave, "clear_sound.wav");
        UnloadWave(wave);
        
        auto& assets = AssetManager::instance();
        assets.init();
        
        assets.get_sound("clear_sound.wav");
        CHECK(assets.sound_count() == 1);
        
        assets.shutdown();
        CHECK(assets.sound_count() == 0);
        
        std::filesystem::remove("clear_sound.wav");
        CloseAudioDevice();
        CloseWindow();
    }
    #endif
}
