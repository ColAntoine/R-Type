/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** SFXPlayer Implementation
*/

#include "ECS/Audio/SFX/SFXPlayer.hpp"
#include <iostream>
#include <cmath>

SFXPlayer::SFXPlayer() : _masterVolume(1.0f) {}

SFXPlayer::~SFXPlayer()
{
    clear();
}

void SFXPlayer::load(const std::string& name, const std::string& path)
{
    if (_soundCache.find(name) != _soundCache.end()) {
        std::cerr << "Sound already loaded: " << name << std::endl;
        return;
    }

    Sound sound = LoadSound(path.c_str());
    if (sound.frameCount == 0) {
        std::cerr << "Failed to load sound: " << path << std::endl;
        return;
    }

    _soundCache[name] = sound;
    std::cout << "Loaded sound: " << name << " from " << path << std::endl;
}

void SFXPlayer::unload(const std::string& name)
{
    auto it = _soundCache.find(name);
    if (it != _soundCache.end()) {
        UnloadSound(it->second);
        _soundCache.erase(it);
        std::cout << "Unloaded sound: " << name << std::endl;
    }
}

void SFXPlayer::clear()
{
    for (auto& [name, sound] : _soundCache) {
        UnloadSound(sound);
    }
    _soundCache.clear();
    std::cout << "Cleared all sounds" << std::endl;
}

void SFXPlayer::play(const std::string& name, float volume)
{
    auto it = _soundCache.find(name);
    if (it == _soundCache.end()) {
        std::cerr << "Sound not found: " << name << std::endl;
        return;
    }

    SetSoundVolume(it->second, volume * _masterVolume);
    PlaySound(it->second);
}

void SFXPlayer::stopAll()
{
    for (auto& [name, sound] : _soundCache) {
        StopSound(sound);
    }
}

void SFXPlayer::setMasterVolume(float volume)
{
   _masterVolume = volume < 0.0f ? 0.0f : (volume > 1.0f ? 1.0f : volume);

    for (auto& [name, sound] : _soundCache) {
        SetSoundVolume(sound, _masterVolume);
    }
}
