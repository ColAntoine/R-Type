/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** AudioManager - Central audio system (singleton)
*/

#pragma once

#include "Music/MusicPlayer.hpp"
#include "SFX/SFXPlayer.hpp"

class AudioManager {
    public:
        static AudioManager& instance();

        void init();
        void shutdown();
        void update();

        MusicPlayer& get_music() { return music_player_; }
        SFXPlayer& get_sfx() { return sfx_player_; }

        void set_master_volume(float volume);
        float get_master_volume() const { return master_volume_; }

        void mute();
        void unmute();
        bool is_muted() const { return is_muted_; }

    private:
        AudioManager()
            : master_volume_(1.0f)
            , is_muted_(false)
            , prev_volume_(1.0f) {}
        ~AudioManager() = default;
        AudioManager(const AudioManager&) = delete;
        AudioManager& operator=(const AudioManager&) = delete;

        MusicPlayer music_player_;
        SFXPlayer sfx_player_;
        float master_volume_;
        bool is_muted_;
        float prev_volume_;
};
