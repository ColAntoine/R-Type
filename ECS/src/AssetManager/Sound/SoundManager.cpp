/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** SoundManager Implementation
*/

#include "ECS/AssetManager/Sound/SoundManager.hpp"
#include <iostream>

Sound* SoundManager::load(const std::string& path) {
    auto it = cache_.find(path);
    if (it != cache_.end()) {
        return &it->second;
    }

    Sound snd = LoadSound(path.c_str());
    if (snd.frameCount == 0) {
        std::cerr << "Failed to load sound: " << path << std::endl;
        return nullptr;
    }

    cache_[path] = snd;
    std::cout << "Loaded sound: " << path << std::endl;
    return &cache_[path];
}

void SoundManager::unload(const std::string& path) {
    auto it = cache_.find(path);
    if (it != cache_.end()) {
        UnloadSound(it->second);
        cache_.erase(it);
        std::cout << "Unloaded sound: " << path << std::endl;
    }
}

void SoundManager::clear() {
    for (auto& [path, snd] : cache_) {
        UnloadSound(snd);
    }
    cache_.clear();
    std::cout << "Cleared all sounds" << std::endl;
}
