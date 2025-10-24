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
        class ScreenInfos {
            public:
                int getHeight() const { return _height; }
                int getWidth() const { return _width; }
                int getFps() const { return _fps; }

                void setHeight(int height) { _height = height; }
                void setWidth(int width) { _width = width; }
                void setFps(int fps) { _fps = fps; }
            private:
                int _height;
                int _width;
                int _fps;
        };

        static RenderManager& instance();

        void init(const char *title);
        void shutdown();

        void begin_frame();
        void end_frame();

        Camera2D_ECS& get_camera() { return camera_; }
        SpriteBatch& get_batch() { return batch_; }

        void set_clear_color(Color color) { clear_color_ = color; }
        Color get_clear_color() const { return clear_color_; }
        ScreenInfos get_screen_infos() const { return _winInfos; }
        Font get_font() const { return _font; }

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
        bool load_font(const char *fontPath);
        void unload_font();

        int scalePosX(int x) const;
        int scalePosY(int y) const;
        int scaleSizeW(int w) const;
        int scaleSizeH(int h) const;
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
        ScreenInfos _winInfos;
        Font _font;
};
