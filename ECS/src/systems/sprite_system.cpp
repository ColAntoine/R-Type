#include <raylib.h>
#include <iostream>
#include "ECS/Systems/Sprite.hpp"
#include "ECS/Registry.hpp"
#include "ECS/Components.hpp"
#include "ECS/Zipper.hpp"

// Static default texture - will be created once
static Texture2D default_texture = {0};

Texture2D SpriteRenderSystem::get_default_texture() {
    // Create default texture if it doesn't exist
    if (default_texture.id == 0) {
        Image img = GenImageColor(1, 1, WHITE);
        default_texture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    return default_texture;
}

Texture2D SpriteRenderSystem::load_texture(const std::string& path) {
    auto it = texture_cache_.find(path);
    if (it != texture_cache_.end()) {
        return it->second;
    }

    // Check if Raylib is ready before loading texture
    if (!IsWindowReady()) {
        std::cerr << "Window not ready, cannot load texture: " << path << std::endl;
        Texture2D fallback = get_default_texture();
        texture_cache_[path] = fallback;
        return fallback;
    }

    if (!FileExists(path.c_str())) {
        std::cerr << "Texture file not found: " << path << std::endl;
        Texture2D fallback = get_default_texture();
        texture_cache_[path] = fallback;
        return fallback;
    }

    Texture2D texture = LoadTexture(path.c_str());
    if (texture.id == 0) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        Texture2D fallback = get_default_texture();
        texture_cache_[path] = fallback;
        return fallback;
    } else {
        std::cout << "Loaded texture: " << path << " (ID: " << texture.id << ")" << std::endl;
    }

    texture_cache_[path] = texture;
    return texture;
}

void SpriteRenderSystem::update(registry& r, float dt) {
    if (!IsWindowReady()) {
        return;
    }

    auto *pos_arr = r.get_if<position>();
    auto *sprite_arr = r.get_if<sprite>();
    auto *collider_arr = r.get_if<collider>();
    Vector2 origin = {0.0f, 0.0f};
    Rectangle dest;
    
    if (!pos_arr || !sprite_arr) return;

    for (auto [p, s, entity] : zipper(*pos_arr, *sprite_arr)) {
        if (!s.visible) continue;

        Texture2D texture = load_texture(s.texture_path);
        if (texture.id == 0) continue;

        float display_width = s.width * s.scale_x;
        float display_height = s.height * s.scale_y;

        Rectangle source = {
            (float)s.frame_x, (float)s.frame_y,
            (s.frame_x == 0 && s.frame_y == 0) ? (float)texture.width : s.width,
            (s.frame_x == 0 && s.frame_y == 0) ? (float)texture.height : s.height
        };

        if (collider_arr && collider_arr->size() > static_cast<size_t>(entity)) {
            auto& collider_comp = (*collider_arr)[static_cast<size_t>(entity)];
            float collider_center_x = p.x + collider_comp.offset_x + collider_comp.w / 2.0f;
            float collider_center_y = p.y + collider_comp.offset_y + collider_comp.h / 2.0f;
            dest = {
                collider_center_x - display_width / 2.0f,
                collider_center_y - display_height / 2.0f,
                display_width, display_height
            };
        } else {
            dest = {
                p.x, p.y,
                display_width, display_height
            };
        }

        DrawTexturePro(texture, source, dest, origin, s.rotation, WHITE);
    }
}

SpriteRenderSystem::~SpriteRenderSystem() {
    texture_cache_.clear();
    // Note: Don't unload default_texture here as other instances might use it
}

extern "C"
std::unique_ptr<ISystem> create_system() {
    return std::make_unique<SpriteRenderSystem>();
}