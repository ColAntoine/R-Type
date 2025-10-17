/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** SoundManager - Manages sound loading and caching
*/

#pragma once

#include <unordered_map>
#include <string>
#include <raylib.h>

class SoundManager {
    public:
        Sound* load(const std::string& path);
        void unload(const std::string& path);
        void clear();
        size_t count() const { return cache_.size(); }

    private:
        std::unordered_map<std::string, Sound> cache_;
};
