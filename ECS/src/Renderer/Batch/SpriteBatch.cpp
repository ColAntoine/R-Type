/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** SpriteBatch Implementation
*/

#include "ECS/Renderer/Batch/SpriteBatch.hpp"
#include <iostream>

void SpriteBatch::begin() {
    if (is_begun_) {
        std::cerr << "SpriteBatch::begin() called twice without end()" << std::endl;
        return;
    }
    draw_calls_.clear();
    is_begun_ = true;
}

void SpriteBatch::draw(Texture2D* texture, Rectangle source, Rectangle dest,
                       Vector2 origin, float rotation, Color tint, int layer) {
    if (!is_begun_) {
        std::cerr << "SpriteBatch::draw() called without begin()" << std::endl;
        return;
    }
    if (!texture || texture->id == 0) return;

    draw_calls_.emplace_back(*texture, source, dest, origin, rotation, tint, layer);  // Dereference pointer, store by value
}

void SpriteBatch::draw(Texture2D* texture, Vector2 position, Color tint, int layer) {
    if (!texture || texture->id == 0) return;

    Rectangle source = {0, 0, (float)texture->width, (float)texture->height};
    Rectangle dest = {position.x, position.y, (float)texture->width, (float)texture->height};
    Vector2 origin = {0, 0};

    draw(texture, source, dest, origin, 0.0f, tint, layer);
}

void SpriteBatch::end() {
    if (!is_begun_) {
        std::cerr << "SpriteBatch::end() called without begin()" << std::endl;
        return;
    }

    if (sort_by_layer_) {
        std::sort(draw_calls_.begin(), draw_calls_.end(),
                  [](const SpriteDrawCall& a, const SpriteDrawCall& b) {
                      return a.layer < b.layer;
                  });
    }

    is_begun_ = false;
}

void SpriteBatch::flush() {
    for (const auto& call : draw_calls_) {
        DrawTexturePro(call.texture, call.source, call.dest,
                       call.origin, call.rotation, call.tint);
    }
    draw_calls_.clear();
}
