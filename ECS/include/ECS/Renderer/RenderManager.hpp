/*
** EPITECH PROJECT, 2025
** R-Type ECS
** File description:
** RenderManager - Central rendering system (singleton)
*/

#pragma once

#include "Camera/Camera.hpp"
#include "Batch/SpriteBatch.hpp"
#include <raylib.h>

class RenderManager {
    public:
        static RenderManager& instance();

        void init(int width, int height, const char* title);
        void shutdown();

        void begin_frame();
        void end_frame();

        Camera2D_ECS& get_camera() { return camera_; }
        SpriteBatch& get_batch() { return batch_; }

        void set_clear_color(Color color) { clear_color_ = color; }
        Color get_clear_color() const { return clear_color_; }

        bool should_close() const;
        int get_fps() const;
        void draw_text(const char *text, int posX, int posY, int fontSize, Color color) const;
        void draw_text_ex(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint) const;
        void draw_circle(int centerX, int centerY, float radius, Color color);
        void draw_rectangle(int posX, int posY, int width, int height, Color color);
        void draw_rectangle_lines_ex(Rectangle rec, float lineThick, Color color);
        void draw_sprite(Texture2D* texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint, int layer = 0);
        bool is_window_ready() const;
        bool window_should_close() const;
    private:
        RenderManager()
            : clear_color_(BLACK)
            , camera_()
            , batch_() {
            batch_.set_sort_by_layer(true);
        }
        ~RenderManager() = default;
        RenderManager(const RenderManager&) = delete;
        RenderManager& operator=(const RenderManager&) = delete;

        Color clear_color_;
        Camera2D_ECS camera_;
        SpriteBatch batch_;
};
