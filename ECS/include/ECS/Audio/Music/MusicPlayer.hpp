/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** MusicPlayer - Music streaming and playback control
*/

#pragma once

#include "ECS/Audio/ISoundPlayer.hpp"


class MusicPlayer : public ISoundPlayer {
    public:
        MusicPlayer();
        ~MusicPlayer();

        void load(const std::string& name, const std::string& path) override;
        void unload(const std::string& name) override;
        void clear() override;

        void play(const std::string& name, float volume = 1.0f) override;
        void stopAll() override;

        void setMasterVolume(float volume) override;
        float getMasterVolume() const override { return _masterVolume; }

        void update();

        std::unordered_map<std::string, Music>& getMusicCache() { return _musicCache; }
        void setMusicCache(const std::unordered_map<std::string, Music>& cache) { _musicCache = cache; }
    private:
        std::unordered_map<std::string, Music> _musicCache;
        float _masterVolume;
};
