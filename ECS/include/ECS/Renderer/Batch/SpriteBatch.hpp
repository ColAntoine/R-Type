/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** SpriteBatch - Efficient sprite rendering with batching
*/

#pragma once

#include <raylib.h>
#include <vector>
#include <algorithm>

struct SpriteDrawCall {
    Texture2D texture;  // Store by value, not pointer!
    Rectangle source;
    Rectangle dest;
    Vector2 origin;
    float rotation;
    Color tint;
    int layer;

    SpriteDrawCall(Texture2D tex, Rectangle src, Rectangle dst,
                   Vector2 org, float rot, Color t, int l = 0)
        : texture(tex), source(src), dest(dst), origin(org),
          rotation(rot), tint(t), layer(l) {}
};

class SpriteBatch {
    public:
        SpriteBatch() : sort_by_layer_(false), is_begun_(false) {}

        void begin();
        void draw(Texture2D* texture, Rectangle source, Rectangle dest,
                Vector2 origin, float rotation, Color tint, int layer = 0);
        void draw(Texture2D* texture, Vector2 position, Color tint, int layer = 0);
        void end();
        void flush();

        void set_sort_by_layer(bool sort) { sort_by_layer_ = sort; }
        size_t get_draw_call_count() const { return draw_calls_.size(); }

    private:
        std::vector<SpriteDrawCall> draw_calls_;
        bool sort_by_layer_;
        bool is_begun_;
};
