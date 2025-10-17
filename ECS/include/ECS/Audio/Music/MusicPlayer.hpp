/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** MusicPlayer - Music streaming and playback control
*/

#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>

class MusicPlayer {
    public:
        MusicPlayer();
        ~MusicPlayer();

        void load(const std::string& name, const std::string& path);
        void unload(const std::string& name);
        void clear();

        void play(const std::string& name, bool loop = true);
        void stop();
        void pause();
        void resume();
        void update();

        void set_volume(float volume);
        void fade_in(float duration);
        void fade_out(float duration);

        bool is_playing() const;
        float get_volume() const { return volume_; }
        std::string get_current() const { return current_music_; }

    private:
        std::unordered_map<std::string, Music> music_cache_;
        std::string current_music_;
        float volume_;
        bool is_fading_;
        float fade_target_;
        float fade_speed_;
};
