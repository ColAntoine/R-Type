/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** AudioManager Implementation
*/

#include "ECS/Audio/AudioManager.hpp"
#include <iostream>

AudioManager& AudioManager::instance() {
    static AudioManager instance;
    return instance;
}

void AudioManager::init() {
    if (initialized_) {
        std::cout << "AudioManager already initialized" << std::endl;
        return;
    }

    InitAudioDevice();
    initialized_ = true;
    std::cout << "AudioManager initialized" << std::endl;
}

void AudioManager::shutdown() {
    if (!initialized_) {
        return;
    }

    music_player_.clear();
    sfx_player_.clear();
    CloseAudioDevice();
    initialized_ = false;
    std::cout << "AudioManager shutdown" << std::endl;
}

void AudioManager::update() {
    music_player_.update();
}

void AudioManager::set_master_volume(float volume) {
    master_volume_ = std::clamp(volume, 0.0f, 1.0f);

    music_player_.setMasterVolume(music_player_.getMasterVolume() * master_volume_);
    sfx_player_.setMasterVolume(sfx_player_.getMasterVolume() * master_volume_);
}

void AudioManager::mute() {
    if (!is_muted_) {
        prev_volume_ = master_volume_;
        set_master_volume(0.0f);
        is_muted_ = true;
        std::cout << "Audio muted" << std::endl;
    }
}

void AudioManager::unmute() {
    if (is_muted_) {
        set_master_volume(prev_volume_);
        is_muted_ = false;
        std::cout << "Audio unmuted" << std::endl;
    }
}
