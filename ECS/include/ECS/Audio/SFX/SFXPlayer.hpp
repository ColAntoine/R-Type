/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** SFXPlayer - Sound effects playback and management
*/

#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>

struct SoundInstance {
    Sound* sound;
    float volume;
    bool is_3d;
    Vector2 position;
};

class SFXPlayer {
    public:
        SFXPlayer();
        ~SFXPlayer();

        void load(const std::string& name, const std::string& path);
        void unload(const std::string& name);
        void clear();

        void play(const std::string& name, float volume = 1.0f);
        void play_at(const std::string& name, Vector2 position, float max_distance = 100.0f);
        void stop_all();

        void set_master_volume(float volume);
        void set_listener_position(Vector2 position);

        float get_master_volume() const { return master_volume_; }
        size_t get_sound_count() const { return sound_cache_.size(); }

    private:
        float calculate_3d_volume(Vector2 sound_pos, float max_distance);

        std::unordered_map<std::string, Sound> sound_cache_;
        float master_volume_;
        Vector2 listener_position_;
};
