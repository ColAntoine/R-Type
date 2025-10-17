/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** SFXPlayer Implementation
*/

#include "ECS/Audio/SFX/SFXPlayer.hpp"
#include <iostream>
#include <cmath>

SFXPlayer::SFXPlayer()
    : master_volume_(1.0f)
    , listener_position_{0.0f, 0.0f} {
}

SFXPlayer::~SFXPlayer() {
    clear();
}

void SFXPlayer::load(const std::string& name, const std::string& path) {
    if (sound_cache_.find(name) != sound_cache_.end()) {
        std::cerr << "Sound already loaded: " << name << std::endl;
        return;
    }

    Sound sound = LoadSound(path.c_str());
    if (sound.frameCount == 0) {
        std::cerr << "Failed to load sound: " << path << std::endl;
        return;
    }

    sound_cache_[name] = sound;
    std::cout << "Loaded sound: " << name << " from " << path << std::endl;
}

void SFXPlayer::unload(const std::string& name) {
    auto it = sound_cache_.find(name);
    if (it != sound_cache_.end()) {
        UnloadSound(it->second);
        sound_cache_.erase(it);
        std::cout << "Unloaded sound: " << name << std::endl;
    }
}

void SFXPlayer::clear() {
    for (auto& [name, sound] : sound_cache_) {
        UnloadSound(sound);
    }
    sound_cache_.clear();
    std::cout << "Cleared all sounds" << std::endl;
}

void SFXPlayer::play(const std::string& name, float volume) {
    auto it = sound_cache_.find(name);
    if (it == sound_cache_.end()) {
        std::cerr << "Sound not found: " << name << std::endl;
        return;
    }

    SetSoundVolume(it->second, volume * master_volume_);
    PlaySound(it->second);
}

void SFXPlayer::play_at(const std::string& name, Vector2 position, float max_distance) {
    auto it = sound_cache_.find(name);
    if (it == sound_cache_.end()) {
        std::cerr << "Sound not found: " << name << std::endl;
        return;
    }

    float volume = calculate_3d_volume(position, max_distance);
    SetSoundVolume(it->second, volume * master_volume_);
    PlaySound(it->second);
}

void SFXPlayer::stop_all() {
    for (auto& [name, sound] : sound_cache_) {
        StopSound(sound);
    }
}

void SFXPlayer::set_master_volume(float volume) {
    master_volume_ = volume < 0.0f ? 0.0f : (volume > 1.0f ? 1.0f : volume);
}

void SFXPlayer::set_listener_position(Vector2 position) {
    listener_position_ = position;
}

float SFXPlayer::calculate_3d_volume(Vector2 sound_pos, float max_distance) {
    float dx = sound_pos.x - listener_position_.x;
    float dy = sound_pos.y - listener_position_.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance >= max_distance) return 0.0f;

    return 1.0f - (distance / max_distance);
}
