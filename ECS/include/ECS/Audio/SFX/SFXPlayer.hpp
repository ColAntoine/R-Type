/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** SFXPlayer - Sound effects playback and management
*/

#pragma once

#include "ECS/Audio/ISoundPlayer.hpp"

class SFXPlayer : public ISoundPlayer {
    public:
        SFXPlayer();
        ~SFXPlayer();

        void load(const std::string& name, const std::string& path) override;
        void unload(const std::string& name) override;
        void clear() override;

        void play(const std::string& name, float volume = 1.0f) override;
        void stopAll() override;

        void setMasterVolume(float volume) override;
        float getMasterVolume() const override { return _masterVolume; }

        std::unordered_map<std::string, Sound>& getSoundCache() { return _soundCache; }
        void setSoundCache(const std::unordered_map<std::string, Sound>& cache) { _soundCache = cache; }
    private:
        std::unordered_map<std::string, Sound> _soundCache;
        float _masterVolume;
};
