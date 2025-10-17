/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** AssetManager Implementation
*/

#include "ECS/AssetManager/AssetManager.hpp"
#include <iostream>

AssetManager& AssetManager::instance() {
    static AssetManager instance;
    return instance;
}

void AssetManager::init() {
    InitAudioDevice();
    std::cout << "AssetManager initialized" << std::endl;
}

void AssetManager::shutdown() {
    textures_.clear();
    sounds_.clear();
    CloseAudioDevice();
    std::cout << "AssetManager shutdown" << std::endl;
}

Texture2D* AssetManager::get_texture(const std::string& path) {
    return textures_.load(path);
}

Sound* AssetManager::get_sound(const std::string& path) {
    return sounds_.load(path);
}

void AssetManager::unload_texture(const std::string& path) {
    textures_.unload(path);
}

void AssetManager::unload_sound(const std::string& path) {
    sounds_.unload(path);
}