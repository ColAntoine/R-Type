#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

// Must be before raylib.h to avoid Windows GDI conflicts
#define NOGDI
#define NOMINMAX

#ifdef ENABLE_AUDIO_TESTS

#include "ECS/AssetManager/Sound/SoundManager.hpp"
#include <raylib.h>
#include <filesystem>

// Helper: Create a real WAV sound using Raylib
static void create_test_sound(const std::string& path) {
    Wave wave = {
        .frameCount = 44100,
        .sampleRate = 44100,
        .sampleSize = 16,
        .channels = 1,
        .data = calloc(44100 * 2, 1)
    };
    ExportWave(wave, path.c_str());
    UnloadWave(wave);
}

TEST_SUITE("SoundManager Tests") {
    TEST_CASE("SoundManager initializes with zero sounds") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        InitAudioDevice();
        
        SoundManager manager;
        CHECK(manager.count() == 0);
        
        CloseAudioDevice();
        CloseWindow();
    }

    TEST_CASE("SoundManager loads sound successfully") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        InitAudioDevice();
        
        create_test_sound("test_sound.wav");
        
        SoundManager manager;
        Sound* sound = manager.load("test_sound.wav");
        
        REQUIRE(sound != nullptr);
        CHECK(manager.count() == 1);
        
        manager.clear();
        std::filesystem::remove("test_sound.wav");
        CloseAudioDevice();
        CloseWindow();
    }

    TEST_CASE("SoundManager caches sounds") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        InitAudioDevice();
        
        create_test_sound("cached_sound.wav");
        
        SoundManager manager;
        Sound* sound1 = manager.load("cached_sound.wav");
        Sound* sound2 = manager.load("cached_sound.wav");
        
        REQUIRE(sound1 != nullptr);
        REQUIRE(sound2 != nullptr);
        CHECK(sound1 == sound2); // Same pointer
        CHECK(manager.count() == 1); // Only 1 sound
        
        manager.clear();
        std::filesystem::remove("cached_sound.wav");
        CloseAudioDevice();
        CloseWindow();
    }

    TEST_CASE("SoundManager loads multiple different sounds") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        InitAudioDevice();
        
        create_test_sound("sound1.wav");
        create_test_sound("sound2.wav");
        create_test_sound("sound3.wav");
        
        SoundManager manager;
        Sound* sound1 = manager.load("sound1.wav");
        Sound* sound2 = manager.load("sound2.wav");
        Sound* sound3 = manager.load("sound3.wav");
        
        REQUIRE(sound1 != nullptr);
        REQUIRE(sound2 != nullptr);
        REQUIRE(sound3 != nullptr);
        CHECK(manager.count() == 3);
        
        // All should be different pointers
        CHECK(sound1 != sound2);
        CHECK(sound2 != sound3);
        CHECK(sound1 != sound3);
        
        manager.clear();
        std::filesystem::remove("sound1.wav");
        std::filesystem::remove("sound2.wav");
        std::filesystem::remove("sound3.wav");
        CloseAudioDevice();
        CloseWindow();
    }

    TEST_CASE("SoundManager unloads sound correctly") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        InitAudioDevice();
        
        create_test_sound("unload_sound.wav");
        
        SoundManager manager;
        manager.load("unload_sound.wav");
        CHECK(manager.count() == 1);
        
        manager.unload("unload_sound.wav");
        CHECK(manager.count() == 0);
        
        manager.clear();
        std::filesystem::remove("unload_sound.wav");
        CloseAudioDevice();
        CloseWindow();
    }

    TEST_CASE("SoundManager unload non-existent sound") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        InitAudioDevice();
        
        SoundManager manager;
        
        // Should not crash
        manager.unload("nonexistent.wav");
        CHECK(manager.count() == 0);
        
        CloseAudioDevice();
        CloseWindow();
    }

    TEST_CASE("SoundManager clear removes all sounds") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        InitAudioDevice();
        
        create_test_sound("clear1.wav");
        create_test_sound("clear2.wav");
        
        SoundManager manager;
        manager.load("clear1.wav");
        manager.load("clear2.wav");
        CHECK(manager.count() == 2);
        
        manager.clear();
        CHECK(manager.count() == 0);
        
        std::filesystem::remove("clear1.wav");
        std::filesystem::remove("clear2.wav");
        CloseAudioDevice();
        CloseWindow();
    }

    TEST_CASE("SoundManager reload after unload") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        InitAudioDevice();
        
        create_test_sound("reload_sound.wav");
        
        SoundManager manager;
        Sound* sound1 = manager.load("reload_sound.wav");
        REQUIRE(sound1 != nullptr);
        
        manager.unload("reload_sound.wav");
        CHECK(manager.count() == 0);
        
        Sound* sound2 = manager.load("reload_sound.wav");
        REQUIRE(sound2 != nullptr);
        CHECK(manager.count() == 1);
        
        manager.clear();
        std::filesystem::remove("reload_sound.wav");
        CloseAudioDevice();
        CloseWindow();
    }

    TEST_CASE("SoundManager load missing file returns nullptr") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        InitAudioDevice();
        
        SoundManager manager;
        Sound* sound = manager.load("does_not_exist.wav");
        
        CHECK(sound == nullptr);
        CHECK(manager.count() == 0);
        
        CloseAudioDevice();
        CloseWindow();
    }

    TEST_CASE("SoundManager count accuracy") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        InitAudioDevice();
        
        create_test_sound("count1.wav");
        create_test_sound("count2.wav");
        
        SoundManager manager;
        CHECK(manager.count() == 0);
        
        manager.load("count1.wav");
        CHECK(manager.count() == 1);
        
        manager.load("count1.wav"); // Load same again
        CHECK(manager.count() == 1); // Still 1 (cached)
        
        manager.load("count2.wav");
        CHECK(manager.count() == 2);
        
        manager.unload("count1.wav");
        CHECK(manager.count() == 1);
        
        manager.unload("count2.wav");
        CHECK(manager.count() == 0);
        
        std::filesystem::remove("count1.wav");
        std::filesystem::remove("count2.wav");
        CloseAudioDevice();
        CloseWindow();
    }

    TEST_CASE("SoundManager handles many sounds") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        InitAudioDevice();
        
        SoundManager manager;
        
        // Create and load 10 sounds (less than textures to save time)
        for (int i = 0; i < 10; i++) {
            std::string filename = "many_sound_" + std::to_string(i) + ".wav";
            create_test_sound(filename);
            Sound* sound = manager.load(filename);
            CHECK(sound != nullptr);
        }
        
        CHECK(manager.count() == 10);
        
        // Clear all
        manager.clear();
        CHECK(manager.count() == 0);
        
        // Cleanup files
        for (int i = 0; i < 10; i++) {
            std::string filename = "many_sound_" + std::to_string(i) + ".wav";
            std::filesystem::remove(filename);
        }
        
        CloseAudioDevice();
        CloseWindow();
    }

    TEST_CASE("SoundManager multiple clear calls") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        InitAudioDevice();
        
        create_test_sound("multi_clear.wav");
        
        SoundManager manager;
        manager.load("multi_clear.wav");
        
        manager.clear();
        CHECK(manager.count() == 0);
        
        // Clear again (should not crash)
        manager.clear();
        CHECK(manager.count() == 0);
        
        std::filesystem::remove("multi_clear.wav");
        CloseAudioDevice();
        CloseWindow();
    }

    TEST_CASE("SoundManager mixed load/unload operations") {
        SetTraceLogLevel(LOG_ERROR);
        InitWindow(1, 1, "Test");
        InitAudioDevice();
        
        create_test_sound("mix1.wav");
        create_test_sound("mix2.wav");
        create_test_sound("mix3.wav");
        
        SoundManager manager;
        
        manager.load("mix1.wav");
        CHECK(manager.count() == 1);
        
        manager.load("mix2.wav");
        CHECK(manager.count() == 2);
        
        manager.unload("mix1.wav");
        CHECK(manager.count() == 1);
        
        manager.load("mix3.wav");
        CHECK(manager.count() == 2);
        
        manager.load("mix1.wav"); // Reload mix1
        CHECK(manager.count() == 3);
        
        manager.clear();
        CHECK(manager.count() == 0);
        
        std::filesystem::remove("mix1.wav");
        std::filesystem::remove("mix2.wav");
        std::filesystem::remove("mix3.wav");
        CloseAudioDevice();
        CloseWindow();
    }
}

#else

TEST_SUITE("SoundManager Tests") {
    TEST_CASE("SoundManager tests disabled" * doctest::description("Audio tests disabled - compile with -DENABLE_AUDIO_TESTS to enable")) {
        CHECK(true);
    }
}

#endif
