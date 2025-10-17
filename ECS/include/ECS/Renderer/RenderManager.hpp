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
