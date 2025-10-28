#include "ECS/Audio/Music/MusicPlayer.hpp"
#include <iostream>

MusicPlayer::MusicPlayer() : _masterVolume(1.0f) {}

MusicPlayer::~MusicPlayer() {
    clear();
}

void MusicPlayer::load(const std::string& name, const std::string& path) {
    if (_musicCache.find(name) != _musicCache.end()) {
        std::cerr << "Music already loaded: " << name << std::endl;
        return;
    }
    _musicCache[name] = LoadMusicStream(path.c_str());
    std::cout << "Loaded music: " << name << std::endl;
}

void MusicPlayer::unload(const std::string& name) {
    auto it = _musicCache.find(name);
    if (it != _musicCache.end()) {
        UnloadMusicStream(it->second);
        _musicCache.erase(it);
        std::cout << "Unloaded music: " << name << std::endl;
    }
}

void MusicPlayer::clear() {
    for (auto& [name, music] : _musicCache) {
        UnloadMusicStream(music);
    }
    _musicCache.clear();
    std::cout << "Cleared all music" << std::endl;
}

void MusicPlayer::play(const std::string& name, float volume) {
    auto it = _musicCache.find(name);
    if (it != _musicCache.end()) {
        SetMusicVolume(it->second, volume * _masterVolume);
        PlayMusicStream(it->second);
        std::cout << "Playing music: " << name << std::endl;
    }
}

void MusicPlayer::stopAll() {
    for (auto& [name, music] : _musicCache) {
        StopMusicStream(music);
    }
    std::cout << "Stopped all music" << std::endl;
}

void MusicPlayer::setMasterVolume(float volume) {
    _masterVolume = volume < 0.0f ? 0.0f : (volume > 1.0f ? 1.0f : volume);
    for (auto& [name, music] : _musicCache) {
        SetMusicVolume(music, _masterVolume);
    }

    std::cout << "Set Music effective volume to: " << _masterVolume << std::endl;
}

void MusicPlayer::update() {
    for (auto& [name, music] : _musicCache) {
        if (IsMusicStreamPlaying(music)) {
            UpdateMusicStream(music);
        }
    }
}
