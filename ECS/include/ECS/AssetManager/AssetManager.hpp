/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** AssetManager - Central asset management (singleton)
*/

#pragma once

#include "Texture/TextureManager.hpp"
#include "Sound/SoundManager.hpp"

class AssetManager {
    public:
        static AssetManager& instance();

        void init();
        void shutdown();

        Texture2D* get_texture(const std::string& path);
        Sound* get_sound(const std::string& path);

        void unload_texture(const std::string& path);
        void unload_sound(const std::string& path);

        size_t texture_count() const { return textures_.count(); }
        size_t sound_count() const { return sounds_.count(); }

    private:
        AssetManager() = default;
        ~AssetManager() = default;
        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;

        TextureManager textures_;
        SoundManager sounds_;
};
