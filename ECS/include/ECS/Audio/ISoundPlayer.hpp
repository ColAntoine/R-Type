#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>

class ISoundPlayer {
    public:
        virtual ~ISoundPlayer() = default;

        virtual void load(const std::string& name, const std::string& path) = 0;
        virtual void unload(const std::string& name) = 0;
        virtual void clear() = 0;

        virtual void play(const std::string& name, float volume = 1.0f) = 0;
        virtual void stopAll() = 0;

        virtual void setMasterVolume(float volume) = 0;
        virtual float getMasterVolume() const = 0;
};