/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** AudioManager - Central audio system (singleton)
*/

#pragma once

#include "Music/MusicPlayer.hpp"
#include "SFX/SFXPlayer.hpp"
#include <algorithm>

class AudioManager {
    public:
        static AudioManager& instance();

        void init();
        void shutdown();
        void update();

        MusicPlayer& get_music() { return music_player_; }
        SFXPlayer& get_sfx() { return sfx_player_; }

        void set_master_volume(float volume);
        void set_music_volume(float volume);
        void set_sfx_volume(float volume);

        float get_master_volume() const { return master_volume_; }
        float get_music_volume() const { return music_volume_; }
        float get_sfx_volume() const { return sfx_volume_; }

        float get_effective_volume(float master, float specific) const;
        void mute();
        void unmute();
        bool is_muted() const { return is_muted_; }

        bool is_initialized() const { return initialized_; }

    private:
        AudioManager()
            : master_volume_(1.0f)
            , is_muted_(false)
            , prev_volume_(1.0f)
            , initialized_(false) {}
        ~AudioManager() = default;
        AudioManager(const AudioManager&) = delete;
        AudioManager& operator=(const AudioManager&) = delete;

        MusicPlayer music_player_;
        SFXPlayer sfx_player_;

        float master_volume_ = 1.0f;
        float music_volume_ = 1.0f; // The value is stored here to get it independently of master volume. The ratio is applied when setting music volume.
        float sfx_volume_ = 1.0f;   // The value is stored here to get it independently of master volume. The ratio is applied when setting SFX volume.

        bool is_muted_;
        float prev_volume_;
        bool initialized_;
};
