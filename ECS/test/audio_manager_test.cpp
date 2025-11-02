/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** AudioManager Unit Tests
*/

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

// Must be before raylib.h to avoid Windows GDI conflicts
#define NOGDI
#define NOMINMAX

#include "ECS/Audio/AudioManager.hpp"
#include <raylib.h>

TEST_SUITE("AudioManager Tests") {
    TEST_CASE("AudioManager is a singleton") {
        auto& instance1 = AudioManager::instance();
        auto& instance2 = AudioManager::instance();
        CHECK(&instance1 == &instance2);
    }

    TEST_CASE("AudioManager initializes successfully") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& audio = AudioManager::instance();
        audio.init();
        
        CHECK(audio.is_initialized());
        
        audio.shutdown();
    }

    TEST_CASE("AudioManager can be initialized multiple times safely") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& audio = AudioManager::instance();
        audio.init();
        audio.init(); // Should not crash
        
        CHECK(audio.is_initialized());
        
        audio.shutdown();
    }

    TEST_CASE("AudioManager master volume can be set and retrieved") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& audio = AudioManager::instance();
        audio.init();
        
        audio.set_master_volume(0.5f);
        CHECK(audio.get_master_volume() == doctest::Approx(0.5f));
        
        audio.set_master_volume(0.0f);
        CHECK(audio.get_master_volume() == doctest::Approx(0.0f));
        
        audio.set_master_volume(1.0f);
        CHECK(audio.get_master_volume() == doctest::Approx(1.0f));
        
        // Test clamping
        audio.set_master_volume(2.0f);
        CHECK(audio.get_master_volume() <= 1.0f);
        
        audio.set_master_volume(-0.5f);
        CHECK(audio.get_master_volume() >= 0.0f);
        
        audio.shutdown();
    }

    TEST_CASE("AudioManager music volume can be set and retrieved") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& audio = AudioManager::instance();
        audio.init();
        
        audio.set_music_volume(0.7f);
        CHECK(audio.get_music_volume() == doctest::Approx(0.7f));
        
        audio.set_music_volume(0.0f);
        CHECK(audio.get_music_volume() == doctest::Approx(0.0f));
        
        audio.set_music_volume(1.0f);
        CHECK(audio.get_music_volume() == doctest::Approx(1.0f));
        
        audio.shutdown();
    }

    TEST_CASE("AudioManager sfx volume can be set and retrieved") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& audio = AudioManager::instance();
        audio.init();
        
        audio.set_sfx_volume(0.8f);
        CHECK(audio.get_sfx_volume() == doctest::Approx(0.8f));
        
        audio.set_sfx_volume(0.0f);
        CHECK(audio.get_sfx_volume() == doctest::Approx(0.0f));
        
        audio.set_sfx_volume(1.0f);
        CHECK(audio.get_sfx_volume() == doctest::Approx(1.0f));
        
        audio.shutdown();
    }

    TEST_CASE("AudioManager get_effective_volume calculates correctly") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& audio = AudioManager::instance();
        audio.init();
        
        float effective = audio.get_effective_volume(0.5f, 0.8f);
        CHECK(effective == doctest::Approx(0.4f)); // 0.5 * 0.8 = 0.4
        
        effective = audio.get_effective_volume(1.0f, 1.0f);
        CHECK(effective == doctest::Approx(1.0f));
        
        effective = audio.get_effective_volume(0.0f, 1.0f);
        CHECK(effective == doctest::Approx(0.0f));
        
        effective = audio.get_effective_volume(1.0f, 0.0f);
        CHECK(effective == doctest::Approx(0.0f));
        
        // Test clamping at upper bound
        effective = audio.get_effective_volume(2.0f, 2.0f);
        CHECK(effective <= 1.0f);
        
        audio.shutdown();
    }

    TEST_CASE("AudioManager mute/unmute functionality works") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& audio = AudioManager::instance();
        audio.init();
        
        audio.set_master_volume(0.5f);
        CHECK_FALSE(audio.is_muted());
        CHECK(audio.get_master_volume() == doctest::Approx(0.5f));
        
        audio.mute();
        CHECK(audio.is_muted());
        CHECK(audio.get_master_volume() == doctest::Approx(0.0f));
        
        audio.unmute();
        CHECK_FALSE(audio.is_muted());
        CHECK(audio.get_master_volume() == doctest::Approx(0.5f)); // Restored
        
        audio.shutdown();
    }

    TEST_CASE("AudioManager mute preserves previous volume") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& audio = AudioManager::instance();
        audio.init();
        
        audio.set_master_volume(0.75f);
        audio.mute();
        audio.unmute();
        
        CHECK(audio.get_master_volume() == doctest::Approx(0.75f));
        
        audio.shutdown();
    }

    TEST_CASE("AudioManager multiple mute calls are safe") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& audio = AudioManager::instance();
        audio.init();
        
        audio.set_master_volume(0.6f);
        audio.mute();
        audio.mute(); // Should not cause issues
        
        CHECK(audio.is_muted());
        
        audio.unmute();
        CHECK(audio.get_master_volume() == doctest::Approx(0.6f));
        
        audio.shutdown();
    }

    TEST_CASE("AudioManager multiple unmute calls are safe") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& audio = AudioManager::instance();
        audio.init();
        
        audio.set_master_volume(0.4f);
        audio.mute();
        audio.unmute();
        audio.unmute(); // Should not cause issues
        
        CHECK_FALSE(audio.is_muted());
        CHECK(audio.get_master_volume() == doctest::Approx(0.4f));
        
        audio.shutdown();
    }

    TEST_CASE("AudioManager music player can be accessed") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& audio = AudioManager::instance();
        audio.init();
        
        auto& music = audio.get_music();
        // Just check we can access it without crash
        CHECK(true);
        
        audio.shutdown();
    }

    TEST_CASE("AudioManager sfx player can be accessed") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& audio = AudioManager::instance();
        audio.init();
        
        auto& sfx = audio.get_sfx();
        // Just check we can access it without crash
        CHECK(true);
        
        audio.shutdown();
    }

    TEST_CASE("AudioManager update can be called") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& audio = AudioManager::instance();
        audio.init();
        
        // Should not crash
        audio.update();
        audio.update();
        audio.update();
        
        CHECK(true);
        
        audio.shutdown();
    }

    TEST_CASE("AudioManager shutdown cleans up properly") {
        SetTraceLogLevel(LOG_ERROR);
        
        auto& audio = AudioManager::instance();
        audio.init();
        
        CHECK(audio.is_initialized());
        
        audio.shutdown();
        CHECK_FALSE(audio.is_initialized());
        
        // Can be re-initialized
        audio.init();
        CHECK(audio.is_initialized());
        
        audio.shutdown();
    }
}
