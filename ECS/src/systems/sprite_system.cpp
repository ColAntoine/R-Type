/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** sprite_system - Integrated with SpriteBatch for performance
*/

#include <raylib.h>
#include <iostream>
#include "ECS/Systems/Sprite.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components.hpp"
#include "ECS/Zipper.hpp"
#include "ECS/Renderer/RenderManager.hpp"

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
        SetTextureFilter(texture, TEXTURE_FILTER_POINT);
    }

    texture_cache_[path] = texture;
    return texture;
}

void SpriteRenderSystem::update(registry& r, float dt) {
    auto *pos_arr = r.get_if<position>();
    auto *sprite_arr = r.get_if<sprite>();
    auto *collider_arr = r.get_if<collider>();
    if (!pos_arr || !sprite_arr) return;

    auto& batch = RenderManager::instance().get_batch();
    Vector2 origin = {0.0f, 0.0f};

    for (auto [p, s, entity] : zipper(*pos_arr, *sprite_arr)) {
        if (!s.visible) continue;

        // Load texture and get pointer to cached texture (not a local copy!)
        load_texture(s.texture_path);  // Ensure it's loaded into cache
        auto it = texture_cache_.find(s.texture_path);
        if (it == texture_cache_.end() || it->second.id == 0) continue; // Failed to load

        Texture2D* texture = &it->second;  // Pointer to cached texture

        // Use the exact dimensions specified in the sprite component
        float display_width = s.width * s.scale_x;
        float display_height = s.height * s.scale_y;

        Rectangle source = {
            (float)s.frame_x, (float)s.frame_y,
            (s.frame_x == 0 && s.frame_y == 0) ? (float)texture->width : s.width,
            (s.frame_x == 0 && s.frame_y == 0) ? (float)texture->height : s.height
        };

        Rectangle dest;
        if (collider_arr && collider_arr->has(static_cast<size_t>(entity))) {
            auto& collider_comp = collider_arr->get(static_cast<size_t>(entity));
            float collider_center_x = p.x + collider_comp.offset_x + collider_comp.w / 2.0f;
            float collider_center_y = p.y + collider_comp.offset_y + collider_comp.h / 2.0f;
            dest = {
                collider_center_x - display_width / 2.0f,
                collider_center_y - display_height / 2.0f,
                display_width, display_height
            };
        } else {
            // No collider: treat position as center and draw centered
            dest = {
                p.x - display_width / 2.0f,
                p.y - display_height / 2.0f,
                display_width, display_height
            };
        }

        // Use SpriteBatch instead of direct DrawTexturePro
        // layer = 0 by default (can be extended with a sprite.layer field later)
        batch.draw(texture, source, dest, origin, s.rotation, WHITE, 0);
    }
    batch.flush();
}

SpriteRenderSystem::~SpriteRenderSystem() {
    // Clear the texture cache without unloading textures
    // Raylib will handle texture cleanup when the graphics context is destroyed
    texture_cache_.clear();
}

DLL_EXPORT ISystem* create_system() {
    try {
        return new SpriteRenderSystem();
    } catch (...) {
        return nullptr;
    }
}

DLL_EXPORT void destroy_system(ISystem* ptr) {
    delete ptr;
}

