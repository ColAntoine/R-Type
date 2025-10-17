/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** MusicPlayer Implementation
*/

#include "ECS/Audio/Music/MusicPlayer.hpp"
#include <iostream>

MusicPlayer::MusicPlayer()
    : current_music_("")
    , volume_(1.0f)
    , is_fading_(false)
    , fade_target_(1.0f)
    , fade_speed_(0.0f) {
}

MusicPlayer::~MusicPlayer() {
    clear();
}

void MusicPlayer::load(const std::string& name, const std::string& path) {
    if (music_cache_.find(name) != music_cache_.end()) {
        std::cerr << "Music already loaded: " << name << std::endl;
        return;
    }

    Music music = LoadMusicStream(path.c_str());
    if (music.frameCount == 0) {
        std::cerr << "Failed to load music: " << path << std::endl;
        return;
    }

    music_cache_[name] = music;
    std::cout << "Loaded music: " << name << " from " << path << std::endl;
}

void MusicPlayer::unload(const std::string& name) {
    auto it = music_cache_.find(name);
    if (it != music_cache_.end()) {
        if (current_music_ == name) {
            stop();
        }
        UnloadMusicStream(it->second);
        music_cache_.erase(it);
        std::cout << "Unloaded music: " << name << std::endl;
    }
}

void MusicPlayer::clear() {
    stop();
    for (auto& [name, music] : music_cache_) {
        UnloadMusicStream(music);
    }
    music_cache_.clear();
    std::cout << "Cleared all music" << std::endl;
}

void MusicPlayer::play(const std::string& name, bool loop) {
    auto it = music_cache_.find(name);
    if (it == music_cache_.end()) {
        std::cerr << "Music not found: " << name << std::endl;
        return;
    }

    if (current_music_ == name && IsMusicStreamPlaying(it->second)) {
        return;
    }

    stop();

    current_music_ = name;
    it->second.looping = loop;
    PlayMusicStream(it->second);
    SetMusicVolume(it->second, volume_);
    std::cout << "Playing music: " << name << std::endl;
}

void MusicPlayer::stop() {
    if (current_music_.empty()) return;

    auto it = music_cache_.find(current_music_);
    if (it != music_cache_.end()) {
        StopMusicStream(it->second);
    }
    current_music_ = "";
}

void MusicPlayer::pause() {
    if (current_music_.empty()) return;

    auto it = music_cache_.find(current_music_);
    if (it != music_cache_.end()) {
        PauseMusicStream(it->second);
    }
}

void MusicPlayer::resume() {
    if (current_music_.empty()) return;

    auto it = music_cache_.find(current_music_);
    if (it != music_cache_.end()) {
        ResumeMusicStream(it->second);
    }
}

void MusicPlayer::update() {
    if (current_music_.empty()) return;

    auto it = music_cache_.find(current_music_);
    if (it != music_cache_.end()) {
        UpdateMusicStream(it->second);

        if (is_fading_) {
            if (volume_ < fade_target_) {
                volume_ += fade_speed_;
                if (volume_ >= fade_target_) {
                    volume_ = fade_target_;
                    is_fading_ = false;
                }
            } else {
                volume_ -= fade_speed_;
                if (volume_ <= fade_target_) {
                    volume_ = fade_target_;
                    is_fading_ = false;
                }
            }
            SetMusicVolume(it->second, volume_);
        }
    }
}

void MusicPlayer::set_volume(float volume) {
    volume_ = volume < 0.0f ? 0.0f : (volume > 1.0f ? 1.0f : volume);

    if (!current_music_.empty()) {
        auto it = music_cache_.find(current_music_);
        if (it != music_cache_.end()) {
            SetMusicVolume(it->second, volume_);
        }
    }
}

void MusicPlayer::fade_in(float duration) {
    if (duration <= 0.0f) return;

    fade_target_ = volume_;
    volume_ = 0.0f;
    fade_speed_ = fade_target_ / (duration * 60.0f); // 60 FPS
    is_fading_ = true;
}

void MusicPlayer::fade_out(float duration) {
    if (duration <= 0.0f) return;

    fade_target_ = 0.0f;
    fade_speed_ = volume_ / (duration * 60.0f); // 60 FPS
    is_fading_ = true;
}

bool MusicPlayer::is_playing() const {
    if (current_music_.empty()) return false;

    auto it = music_cache_.find(current_music_);
    if (it != music_cache_.end()) {
        return IsMusicStreamPlaying(it->second);
    }
    return false;
}
