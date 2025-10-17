/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** TextureManager Implementation
*/

#include "ECS/AssetManager/Texture/TextureManager.hpp"
#include <iostream>

Texture2D* TextureManager::load(const std::string& path) {
    auto it = cache_.find(path);
    if (it != cache_.end()) {
        return &it->second;
    }

    Texture2D tex = LoadTexture(path.c_str());
    if (tex.id == 0) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return nullptr;
    }

    cache_[path] = tex;
    std::cout << "Loaded texture: " << path << std::endl;
    return &cache_[path];
}

void TextureManager::unload(const std::string& path) {
    auto it = cache_.find(path);
    if (it != cache_.end()) {
        UnloadTexture(it->second);
        cache_.erase(it);
        std::cout << "Unloaded texture: " << path << std::endl;
    }
}

void TextureManager::clear() {
    for (auto& [path, tex] : cache_) {
        UnloadTexture(tex);
    }
    cache_.clear();
    std::cout << "Cleared all textures" << std::endl;
}
