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

#define BASE_WIDTH 1920
#define BASE_HEIGHT 1080

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

        bool should_close() const;
        int get_fps() const;
        void draw_text(const char *text, int posX, int posY, int fontSize, Color color) const;
        bool is_window_ready() const;
        bool window_should_close() const;

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
};
