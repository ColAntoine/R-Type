/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** sprite_system
*/

#include <raylib.h>
#include <iostream>
#include "ecs/systems/sprite_system.hpp"
#include "ecs/registry.hpp"
#include "ecs/components.hpp"
#include "ecs/zipper.hpp"

Texture2D SpriteRenderSystem::load_texture(const std::string& path) {
    auto it = texture_cache_.find(path);
    if (it != texture_cache_.end()) {
        return it->second;
    }

    Texture2D texture = LoadTexture(path.c_str());
    if (texture.id == 0) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        // Create a default 1x1 white texture for missing textures
        Image img = GenImageColor(1, 1, WHITE);
        texture = LoadTextureFromImage(img);
        UnloadImage(img);
    } else {
        std::cout << "Loaded texture: " << path << " (ID: " << texture.id << ")" << std::endl;
    }
    
    texture_cache_[path] = texture;
    return texture;
}

void SpriteRenderSystem::update(registry& r, float dt) {
    auto *pos_arr = r.get_if<position>();
    auto *sprite_arr = r.get_if<sprite>();
    if (!pos_arr || !sprite_arr) return;

    for (auto [p, s, entity] : zipper(*pos_arr, *sprite_arr)) {
        if (!s.visible) continue;

        Texture2D texture = load_texture(s.texture_path);
        if (texture.id == 0) continue; // Failed to load

        Rectangle source = {
            (float)s.frame_x, (float)s.frame_y,
            (s.frame_x == 0 && s.frame_y == 0) ? (float)texture.width : s.width,
            (s.frame_x == 0 && s.frame_y == 0) ? (float)texture.height : s.height
        };

        Rectangle dest = {
            p.x, p.y,
            s.width * s.scale_x, s.height * s.scale_y
        };

        Vector2 origin = {0.0f, 0.0f}; // Top-left origin

        DrawTexturePro(texture, source, dest, origin, s.rotation, WHITE);
    }
}

SpriteRenderSystem::~SpriteRenderSystem() {
    // Clear the texture cache without unloading textures
    // Raylib will handle texture cleanup when the graphics context is destroyed
    texture_cache_.clear();
}

std::unique_ptr<ISystem> create_system() {
    return std::make_unique<SpriteRenderSystem>();
}

